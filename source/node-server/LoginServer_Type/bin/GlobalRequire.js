"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.acount_seq_increase = exports.account_seq = exports.DBManager = exports.init = exports.serverConfig = void 0;
function init() {
}
exports.init = init;
exports.serverConfig = require('../serverConfig.json');
exports.DBManager = require('./DBManager');
exports.account_seq = 1;
function acount_seq_increase() {
    exports.account_seq++;
}
exports.acount_seq_increase = acount_seq_increase;
