import React from 'react';
import './App.css';

export default class App extends React.Component {
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

    onSocketMessage(evt) {
        this.setState(state => ({
            textContent: [...state.textContent, evt.data],
        }))
    }

    componentDidMount() {
        this.props.socketRepo.startSocket();
        this.props.socketRepo.setOnOpen(this.onSocketOpen.bind(this));
        this.props.socketRepo.setOnMessage(this.onSocketMessage.bind(this));
        this.props.socketRepo.setOnClose(this.onSocketClose.bind(this));
    }

    executeCommand() {
        this.props.socketRepo.sendCode(this.state.inputString);
        this.setState(state => ({
            inputString: ""
        }));
    }

    render() {
        return (
            <div className="App">
                <div className="outArea">
                    <div className="statusArea">{this.state.connection}</div>
                    <div className="outAreaItem outAreaItemInfo">ピカ狸　ライブ実行</div>
                    {this.state.textContent.map((item, index) => (
                        this.renderItem(item, index)
                    ))}
                    <textarea
                        className={"Text " + (this.state.inputString.includes("\n") ? "multi" : "")}
                        placeholder="入力してください"
                        onKeyDown={(e) => {
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
                        }}
                        value={this.state.inputString}
                        onChange={(e) => this.setState({inputString: e.target.value})}
                    />
                    {this.state.inputString.includes("\n") && (
                        <div className={"outAreaItem smalltext"}>実行はSHIFT＋ENTER</div>
                    )}
                </div>
            </div>
        );
    }

    renderItem(item, index) {
        if(item.startsWith("》")) {
            return (
                <div key={index} className="outAreaItem outAreaItemResult">
                    {item.split("\n").map((line) => (
                        <div>{line}</div>
                    ))}
                </div>
            )
        }
        return (
            <div key={index} className="outAreaItem">
                {item.split("\n").map((line) => (
                    <div>{line}</div>
                ))}
            </div>
        )
    }
}

