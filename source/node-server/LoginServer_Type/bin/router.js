"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.Route = void 0;
function Route(app) {
    app.post('/LoginProcess', require('./Process/LoginProcess').OnPost);
}
exports.Route = Route;
;
