export function Route(app:any) {
    app.post('/LoginProcess', require('./Process/LoginProcess').OnPost);
};