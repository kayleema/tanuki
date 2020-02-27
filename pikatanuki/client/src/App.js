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
        this.socket = new WebSocket("ws://localhost:9002/");
        this.socket.onopen = this.onSocketOpen.bind(this);
        this.socket.onmessage = this.onSocketMessage.bind(this);
        this.socket.onclose = this.onSocketClose.bind(this);
    }

    render() {
        return (
            <div className="App">
                <div className="outArea">
                    <div className="statusArea">{this.state.connection}</div>
                    <div className="outAreaItem">狸語 リアイブ実行</div>
                    <div className="outAreaItem">スタート</div>
                    {this.state.textContent.map((item, index) => (
                        <div key={index} className="outAreaItem">
                            {item}
                        </div>
                    ))}
                    <input
                        className="Text"
                        placeholder="入力してください"
                        onKeyDown={(e) => {
                            if (e.keyCode === 229) {
                                return;
                            }
                            if (e.key === "Enter") {
                                this.socket.send(this.state.inputString)
                                this.setState(state => ({
                                    inputString: ""
                                }));
                            }
                        }}
                        value={this.state.inputString}
                        onChange={(e) => this.setState({inputString: e.target.value})}
                    />
                </div>
            </div>
        );
    }
}

