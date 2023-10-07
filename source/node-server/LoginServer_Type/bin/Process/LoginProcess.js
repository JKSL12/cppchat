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
Object.defineProperty(exports, "__esModule", { value: true });
exports.OnPost = void 0;
const async = require("async");
const Global = __importStar(require("../GlobalRequire"));
function OnPost(req, res, next) {
    var _resultObj = {
        result: false,
        account_seq: -1,
        ip: '',
        port: 0
    };
    async.waterfall([
        function Task1_Login(callback) {
            var id = req.body.id;
            var pwd = req.body.pwd;
            // Global.DBManager.Call_usp_web_get_user_login(id, pwd, function (err:any, result:any) {
            //     if( err !== null) {
            //         callback(1, err + result);
            //         return;
            //     }
            //     callback(null, result);
            // });
            console.log('id : ' + id + ' pwd : ' + pwd);
            var resultObj = {
                'account_seq': Global.account_seq,
            };
            Global.acount_seq_increase();
            callback(null, resultObj);
        },
        function Task2_GetServer(result, callback) {
            result.address = "127.0.0.1:9000";
            callback(null, result);
        }
    ], function onFinal(err, results) {
        if (err !== null) {
            console.warn('LoginProcess Final err : ' + err + ', results:' + results);
            _resultObj.result = false;
            res.write(JSON.stringify(_resultObj));
        }
        else {
            _resultObj.result = true;
            _resultObj.account_seq = results.account_seq;
            var addr = results.address.split(':');
            _resultObj.ip = addr[0];
            _resultObj.port = addr[1];
            res.write(JSON.stringify(_resultObj));
        }
        res.end();
    } // Final.
    );
}
exports.OnPost = OnPost;
