"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (k !== "default" && Object.prototype.hasOwnProperty.call(mod, k)) __createBinding(result, mod, k);
    __setModuleDefault(result, mod);
    return result;
};
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.Call_usp_web_get_user_login = exports.ConnectAllDB = void 0;
const mssql = require("mssql");
const Global = __importStar(require("./GlobalRequire"));
var dbConfig = Global.serverConfig.DB;
var MAX_DB_INDEX_NUM = 16;
var dbConnections = [];
function ConnectAllDB(callBack) {
    var replyCount = 0;
    var successCount = 0;
    for (var id = 1; id <= MAX_DB_INDEX_NUM; id++) {
        ConnectDB(id).then(function onFinish(result) {
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
        }).catch(function (err) {
            console.log(err); // Error: Request is failed
        });
    }
}
exports.ConnectAllDB = ConnectAllDB;
function DisconnectAllDB() {
    for (const key in dbConnections) {
        dbConnections[key].close();
    }
    dbConnections = [];
}
function ConnectDB(dbID) {
    const mssql_config = {
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
    return new Promise((resolve, reject) => __awaiter(this, void 0, void 0, function* () {
        var pool = new mssql.ConnectionPool(mssql_config, (err) => {
            if (err) {
                reject(err);
                return;
            }
        }).connect()
            .then(pool => {
            dbConnections[dbID] = pool;
            resolve({ result: 1, id: dbID });
        });
    }));
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
function Call_usp_web_get_user_login(id, pwd, callback) {
    var dbID = GetGameDBID();
    var connection = dbConnections[dbID];
    // 프로시져 호출!!
    var request = new mssql.Request(connection)
        .input('UserID', mssql.BigInt, 2777073)
        .execute('P_Explor_Count_Get', function on_P_LordTitle_List_Get(err, result, returnValue) {
        try {
            if (err) {
                throw 'on_P_LordTitle_List_Get err: ' + err;
            }
            console.dir(result.recordsets);
            if (typeof (callback) === 'function') {
                callback();
            }
        }
        catch (exception) {
            console.warn(exception);
            callback();
        }
    });
}
exports.Call_usp_web_get_user_login = Call_usp_web_get_user_login;
;
