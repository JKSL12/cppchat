var mssql = require('mssql');
var dbConfig = gServerConfig.DB;
var MAX_DB_INDEX_NUM = 16;

var dbConnections = [];

exports.ConnectAllDB = function ConnectAllDB(callBack) {
    var replyCount = 0;
    var successCount = 0;
    for(var id=1; id<=MAX_DB_INDEX_NUM; id++) {
        ConnectDB(onError, id, function onFinish(result) {
            replyCount++;
            if(result.result === 1) {
                successCount++;
                console.info('ConnectDB(%d)...', result.id);
            }
            if(result.result === 0) {
                console.error('DB Connect Fail Exists. Check DB status.');
                setTimeout(function() {
                    process.exit(0);
                }, 1000);
                return;
            }

            if(replyCount == MAX_DB_INDEX_NUM) {
                console.log('All DB Connect Finished.');
                callBack();
            }
        });
    }    

    function onError(err) {
        console.error('Err:' + err);
    }
}

exports.DisconnectAllDB = function DisconnectAllDB(){
    for(key in dbConnections) {
        dbConnections[key].close();
    }

    dbConnections = [];
}

function ConnectDB(err, dbID, onFinish) {
    try {
        console.log('[ConnectDB]' + dbID + '    ' + dbConfig[dbID].user + ' ' + dbConfig[dbID].pw + '   ' + dbConfig[dbID].ip + '   ' + dbConfig[dbID].port + ' ' + dbConfig[dbID].database)

        if(dbConfig[dbID] == undefined) {
            throw 'dbConfig dbID: ' + dbID + ' is undefined.';
        }

        var config = {
            user: dbConfig[dbID].user,
            password: dbConfig[dbID].pw,
            server:  dbConfig[dbID].ip,
            port:  dbConfig[dbID].port,
            database:  dbConfig[dbID].database,
            requestTimeout : 20000,
            pool: {
                max: 100,
                min: 1,
                idleTimeoutMillis: 30000
            },
            encrypt: false,
            trustServerCertificate: true
        }

        const pool = new mssql.ConnectionPool(config)
          .connect()
          .then(pool => {     
                dbConnections[dbID] = pool;

                onFinish({result:1, id:dbID});
                  })
          .catch(err => console.log('Database Connection Failed! Bad Config: ', err))
    } catch (ex) {
        console.warn("Exception:" + ex);
        onFinish({result:0, id:dbID, cause:'Check DB setting @ serverConfig.json'});
    }
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

exports.Call_usp_web_get_user_login = function usp_web_get_user_login(callback) {
    var dbID = GetGameDBID();
    var connection = dbConnections[dbID];
    // 프로시져 호출!!
    var request = new mssql.Request(connection)
        .input('UserID', mssql.BigInt, 2777073)
        .execute('P_Explor_Count_Get', function on_P_LordTitle_List_Get(err, result, returnValue) {
        try {
            if(err) {
                throw 'on_P_LordTitle_List_Get err: ' + err;
            }

            
            console.dir(result.recordsets);
               
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