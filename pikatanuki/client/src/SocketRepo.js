
export default class SocketRepo {
    startSocket() {
        this.connect();
    }

    connect() {
        this.reconnect();
    }

    reconnect() {
        this.socket = new WebSocket("ws://localhost:9002/");
        this.socket.onopen = this.onopen.bind(this);
        this.socket.onmessage = this.onmessage.bind(this);
        this.socket.onclose = this.onclose.bind(this);
        // this.socket = new WebSocket("ws://ec2-54-250-239-95.ap-northeast-1.compute.amazonaws.com:9002/");
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