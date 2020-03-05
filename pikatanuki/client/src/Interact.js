import React from 'react';

export default class Interact extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            textContent: [],
            inputString: "",
            connection: "接続中"
        };
    }

    onSocketOpen() {
        this.setState({connection: "接続した"})
    }

    onSocketClose() {
        this.setState({connection: "socket closed"})
    }

    onSocketMessage(data) {
        this.setState(state => ({
            textContent: [...state.textContent, data],
        }))
    }

    componentDidMount() {
        this.props.socketRepo.startSocket();
        this.props.socketRepo.setOnOpen(this.onSocketOpen.bind(this));
        this.props.socketRepo.setOnMessage(this.onSocketMessage.bind(this));
        this.props.socketRepo.setOnClose(this.onSocketClose.bind(this));
    }

    executeCommand() {
        const newItem = {
            messageType: "out",
            code: this.state.inputString
        };
        this.setState(state => ({
            textContent: [...state.textContent, newItem],
        }));
        this.props.socketRepo.sendCode(this.state.inputString);
        this.setState(state => ({
            inputString: ""
        }));
    }

    onKeyDown(e) {
        if (e.keyCode === 229) {
            return;
        }
        if (e.key === "Enter") {
            if (
                !(this.state.inputString.startsWith("関数、") ||
                    this.state.inputString.startsWith("もし") ||
                    this.state.inputString.startsWith("あるいは") ||
                    this.state.inputString.startsWith("その他")) ||
                e.getModifierState("Shift")
            ) {
                this.executeCommand();
                e.preventDefault();
            }
        }
    }

    render() {
        return (
            <div className="outArea">
                <div className="statusArea">{this.state.connection}</div>
                <div className="outAreaItem outAreaItemInfo">ピカ狸 ライブ実行</div>
                {this.state.textContent.map((item, index) => (
                    this.renderItem(item, index)
                ))}
                <textarea
                    className={"Text " + (this.state.inputString.includes("\n") ? "multi" : "")}
                    placeholder="入力してください"
                    onKeyDown={this.onKeyDown.bind(this)}
                    value={this.state.inputString}
                    onChange={(e) => this.setState({inputString: e.target.value})}
                />
                {this.state.inputString.includes("\n") && (
                    <div className={"outAreaItem smalltext"}>実行はSHIFT＋ENTER</div>
                )}
            </div>
        );
    }

    renderItem(item, index) {
        if (item.messageType === "result") {
            return (
                <div key={index} className="outAreaItem outAreaItemResult">
                    結果：{item.resultString}
                </div>
            )
        }
        if (item.messageType === "display") {
            return (
                <div key={index} className="outAreaItem outAreaItemDisplay">
                    表示：{item.message}
                </div>
            )
        }
        if (item.messageType === "out") {
            return (
                <div key={index} className="outAreaItem outAreaItemInput">
                    {
                        item.code.split("\n").map((line) => (
                            [line, <br/>]
                        ))
                    }
                </div>
            )
        }
    }
}
