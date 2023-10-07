require('./Common/GlobalRequire');

var express = require('express');
var bodyParser = require('body-parser');
var app = express();
var fs = require('fs');
var http = require('http');
var https = require('https');
var cors = require('cors');

app.disable("x-powered-by");

app.use(bodyParser.urlencoded({extended: false}));
app.use(cors());

var router = require('./router.js');
router.Route(app);

gDBManager.ConnectAllDB(OnListen);
//OnListen();

function OnListen()
{
    var _server = http.createServer(app);
    _server.listen(30000, function onListen() {});

    console.log("start server");
}

setTimeout(function () {
    gDBManager.Call_usp_web_get_user_login(function() { console.log('finish'); });
}, 2000);