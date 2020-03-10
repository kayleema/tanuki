import React from 'react';
import './App.css';
import Interact from "./Interact";
import Edit from "./Edit";

export default class App extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            screen: "interact"
        }
    }

    componentDidMount() {
        this.props.socketRepo.startSocket();
    }

    changeScreen() {
        this.setState({
            screen: (this.state.screen === "edit") ? "interact" : "edit"
        })
    }

    render() {
        return (
            <div className="App">
                <div className="header">
                    ピカ狸・
                    <button onClick={this.changeScreen.bind(this)}>
                        {(this.state.screen === "interact") && "編集画面へ"}
                        {(this.state.screen === "edit") && "ライブ実行画面へ"}
                    </button>
                    {process.env.NODE_ENV == "development" && <em>・！開発環境ですよ！</em>}
                </div>
                {(this.state.screen === "interact") && <Interact socketRepo={this.props.socketRepo}/>}
                {(this.state.screen === "edit") && <Edit socketRepo={this.props.socketRepo}/>}
            </div>
        );
    }
}

