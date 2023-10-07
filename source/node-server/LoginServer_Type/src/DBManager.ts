import mssql = require('mssql');
import * as Global from './GlobalRequire';

var dbConfig = Global.serverConfig.DB;
var MAX_DB_INDEX_NUM = 16;

var dbConnections:any = [];

export function ConnectAllDB(callBack:any) {
    var replyCount = 0;
    var successCount = 0;

    for (var id = 1; id <= MAX_DB_INDEX_NUM; id++) {
        ConnectDB(id).then(function onFinish(result: any) {
            replyCount++;
            if (result.result === 1) {
                successCount++;
                console.info('ConnectDB(%d)...', result.id);
            }
            if (result.result === 0) {
                console.error('DB Connect Fail Exists. Check DB status.');
                setTimeout(function () {
                    process.exit(0);
                }, 1000);
                return;
            }

            if (replyCount == MAX_DB_INDEX_NUM) {
                console.log('All DB Connect Finished.');
                callBack();
            }
        }).catch(function (err:any) {
            console.log(err); // Error: Request is failed
        });
    }
}

function DisconnectAllDB(){
    for(const key in dbConnections) {
        dbConnections[key].close();
    }

    dbConnections = [];
}

function ConnectDB(dbID:number) {
    const mssql_config: mssql.config = {
        user: dbConfig[dbID].user,
        password: dbConfig[dbID].pw,
        server: dbConfig[dbID].ip,
        port: dbConfig[dbID].port,
        database: dbConfig[dbID].database,
        requestTimeout: 20000,
        driver: 'tedious',
        connectionTimeout: 10000,
        pool: {
            max: 100,
            min: 1,
            idleTimeoutMillis: 30000
        },
        options: {
            encrypt: false,
            trustServerCertificate: true
        }
    };

    return new Promise(async (resolve, reject) => {
        var pool = new mssql.ConnectionPool(mssql_config, (err: any) => {
            if (err) {
                reject(err);
                return;
            }            
        }).connect()
        .then(pool => {
            dbConnections[dbID] = pool;

            resolve({ result: 1, id: dbID });
        })
    });
}

function GetGameDBID() {
    try {
        return (Date.now() % MAX_DB_INDEX_NUM) + 1;
    }
    catch (ex) {
        console.warn('GetGameDBID - ex', ex);
        return 1;
    }
}

export function Call_usp_web_get_user_login(id:string, pwd:string, callback:any) {
    var dbID = GetGameDBID();
    var connection = dbConnections[dbID];
    // 프로시져 호출!!
    var request = new mssql.Request(connection)
        .input('Id', mssql.VarChar(50), id)
        .input('Pwd', mssql.VarChar(50), pwd)
        .execute('P_User_Login', function on_P_User_Login(err:any, result:any, returnValue:any) {
        try {
            if(err) {
                throw 'on_P_User_Login err: ' + err;
            }            
               
            if(typeof(callback) === 'function') {
                callback();
            }           
        }
        catch(exception) {
            console.warn(exception);
            callback();
        }
    });
};