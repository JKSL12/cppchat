import express = require('express');
import bodyParser = require('body-parser');
import cors = require('cors');
import * as router from './router';
import * as Global from './GlobalRequire';
import http = require('http');

var app = express();

app.disable("x-powered-by");

app.use(bodyParser.urlencoded({extended: false}));
app.use(cors());

router.Route(app);

//Global.DBManager.ConnectAllDB(OnListen);

OnListen();

function OnListen()
{
    var _server = http.createServer(app);
    _server.listen(30000, function onListen() {});

    console.log("start server");
}