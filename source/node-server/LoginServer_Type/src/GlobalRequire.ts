export let serverConfig : {
    DB : any;
};

export function init() {

}
serverConfig = require('../serverConfig.json');
export let DBManager = require('./DBManager');

export var account_seq:number = 1;
export function acount_seq_increase() {
    account_seq++;
}
