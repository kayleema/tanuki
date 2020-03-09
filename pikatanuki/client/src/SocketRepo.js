
export default class SocketRepo {
    startSocket() {
        this.connect();
    }

    connect() {
        this.socket = new WebSocket("ws://localhost:9002/");
        // this.socket = new WebSocket("ws://ec2-54-250-239-95.ap-northeast-1.compute.amazonaws.com:9002/");
    }

    sendCode(code) {
        this.socket.send(JSON.stringify({
            messageType: "code" ,
            code: code
        }));
    }

    setOnOpen(handler) {
        this.socket.onopen = handler;
    }

    setOnMessage(handler) {
        this.socket.onmessage = (evt) => {
            console.log(evt.data);
            let json = JSON.parse(evt.data);
            handler(json);
        };
    }

    setOnClose(handler) {
        this.socket.onclose = handler;
        setTimeout(() => {
            console.log('reconnecting');
            this.connect();
        }, 1000);
    }
}