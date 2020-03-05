
export default class SocketRepo {
    startSocket() {
        this.socket = new WebSocket("ws://localhost:9002/");
    }

    sendCode(code) {
        this.socket.send(code)
    }

    setOnOpen(handler) {
        this.socket.onopen = handler;
    }
    setOnMessage(handler) {
        this.socket.onmessage = handler;
    }
    setOnClose(handler) {
        this.socket.onclose = handler;
    }
}