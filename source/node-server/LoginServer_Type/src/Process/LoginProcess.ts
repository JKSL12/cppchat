import async = require('async');
import * as Global from '../GlobalRequire';

export function OnPost(req:any, res:any, next:any) 
{
    var _resultObj = {
        result:false,
        account_seq:-1,
        ip:'',
        port:0
    };

    async.waterfall([
            function Task1_Login(callback:any) {
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
                    'account_seq':Global.account_seq,                    
                }

                Global.acount_seq_increase();

                callback(null, resultObj);
            },
            function Task2_GetServer(result:any, callback:any)
            {
                result.address = "127.0.0.1:9000";
                callback(null, result);
            }
        ],
        function onFinal(err:any, results:any) {
            if(err !== null) {
                console.warn('LoginProcess Final err : ' + err + ', results:' + results);
                _resultObj.result = false;
                res.write(JSON.stringify(_resultObj));
            }
            else {
                _resultObj.result= true;
                _resultObj.account_seq =results.account_seq;
                var addr = results.address.split(':');
                _resultObj.ip = addr[0];
                _resultObj.port = addr[1];
                res.write(JSON.stringify(_resultObj));
            }
            res.end();
        } // Final.
    );
}