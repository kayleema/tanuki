
export default class SocketRepo {
    startSocket() {
        this.connect();
    }

    connect() {
        this.reconnect();
    }

    reconnect() {
        if(process.env.NODE_ENV === "development") {
            this.socket = new WebSocket("ws://localhost:9002/");
        } else {
            this.socket = new WebSocket("wss://pikatanuki.kaylee.jp/");
        }
        this.socket.onopen = this.onopen.bind(this);
        this.socket.onmessage = this.onmessage.bind(this);
        this.socket.onclose = this.onclose.bind(this);
    }

    onopen() {
        this.onopenHandler();
    }

    onmessage(evt) {
        // console.log(evt.data);
        let json = JSON.parse(evt.data);
        this.onmessageHandler(json);
    }

    onclose() {
        this.oncloseHandler();
        setTimeout(() => {
            console.log('reconnecting...');
            this.reconnect();
        }, 1000);
    }

    sendCode(code) {
        this.socket.send(JSON.stringify({
            messageType: "code" ,
            code: code
        }));
    }

    sendFile(code) {
        this.socket.send(JSON.stringify({
            messageType: "file",
            code: code
        }));
    }

    setOnOpen(handler) {
        this.onopenHandler = handler;
    }

    setOnMessage(handler) {
        this.onmessageHandler = handler;
    }

    setOnClose(handler) {
        this.oncloseHandler = handler;
    }
}