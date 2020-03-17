
export default class SocketRepo {
    startSocket() {
        this.connect();
    }

    connect() {
        this.reconnect();
    }

    reconnect() {
        clearInterval(this.interval);
        if(process.env.NODE_ENV === "development") {
            this.socket = new WebSocket("ws://localhost:9002/");
            // this.socket = new WebSocket("wss://socktanuki.kaylee.jp/");

        } else {
            this.socket = new WebSocket("wss://socktanuki.kaylee.jp/");
        }
        this.socket.onopen = this.onopen.bind(this);
        this.socket.onmessage = this.onmessage.bind(this);
        this.socket.onclose = this.onclose.bind(this);
        this.socket.onerror = this.onerror.bind(this);
    }

    onopen() {
        this.onopenHandler && this.onopenHandler();
        this.liveFlag = true;
        this.interval = setInterval(() => {
            if (!this.liveFlag) {
                console.log("socket deaaaath");
                this.socket.close();
                return;
            }
            this.liveFlag = false;
            this.socket.send(JSON.stringify({messageType: "ping"}));
        }, 5000);
    }

    onmessage(evt) {
        console.log(evt.data);
        let json = JSON.parse(evt.data);
        if (json.messageType === "pong") {
            this.liveFlag=true;
            return;
        }
        this.onmessageHandler && this.onmessageHandler(json);
    }

    onerror(e) {
        console.log(e);
    }

    onclose() {
        clearInterval(this.interval);
        this.oncloseHandler();
        setTimeout(() => {
            console.log('reconnecting...');
            this.reconnect();
        }, 5000);
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