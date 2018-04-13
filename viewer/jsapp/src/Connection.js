export class Connection {
    State = Object.freeze({connected: 1, notConnected: 2, connecting: 3});
    onConnected = undefined;
    onDisconnected = undefined;
    onMessage = undefined;
    socket = undefined;
    state = this.State.notConnected;
    onConnected = undefined;
    onDisconnected = undefined;
    onMessage = undefined;

    connect(uri, waitForServer) {
        this.socket = new WebSocket(uri);
        this.state = this.State.connecting;
        
        this.socket.onopen = function(event) {
            this.state = this.State.connected;
            this.onConnected();
        }.bind(this);
        this.socket.onclose = function() {
            this.onDisconnected();
            this.state = this.State.notConnected;
        }.bind(this);
        this.socket.onmessage = function(evt) {
            this.onMessage(evt.data);
        }.bind(this);
    }

    disconnect() {
        if (this.state != this.State.notConnected) {
            this.socket.close();
        }
    }

    send(message) {
        this.socket.send(message);
    }
}