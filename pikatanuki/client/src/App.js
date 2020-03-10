import React from 'react';
import './App.css';
import Interact from "./Interact";
import Edit from "./Edit";
import { highlightAllUnder } from 'prismjs';
import GoogleLogin from 'react-google-login';
import GoogleLogout from 'react-google-login';

export default class App extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            screen: "edit",
            googleToken: localStorage.getItem("googleToken"),
            googleEmail: localStorage.getItem("googleEmail"),
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

    login(event) {
        console.log(event)
        localStorage.setItem("googleToken", event.tokenId)
        localStorage.setItem("googleEmail", event.profileObj.email)
        this.setState({
            googleToken: event.tokenId,
            googleEmail: event.profileObj.email,
        })
    }

    logout(event) {
        localStorage.removeItem("googleToken")
        this.setState(
            {googleToken: undefined}
        )
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
                    ・
                    {(this.state.googleToken == undefined) && (
                        <div className="loginContainer">
                            <div className="dialog">
                                <h1>ピカ狸へようこそ</h1>
                                <h2>ログインしませんか</h2>
                                <GoogleLogin
                                    clientId="581212950796-01c78s70trahbfpfq9ds7qahs7gn7aqo.apps.googleusercontent.com"
                                    buttonText="ログイン"
                                    onSuccess={this.login.bind(this)}
                                    onFailure={() => {}}
                                    cookiePolicy={'single_host_origin'}
                                    icon={false}
                                    theme="dark"
                                />
                                <h1>🦝</h1>
                            </div>
                        </div>
                    )}
                    {this.state.googleToken != undefined && (
                        <span>「{this.state.googleEmail}」としてログインされました・</span>
                    )}
                    {this.state.googleToken != undefined && (
                        <button onClick={this.logout.bind(this)}>
                            ログアウト
                        </button>
                    )}
                </div>
                {(this.state.screen === "interact") && <Interact socketRepo={this.props.socketRepo}/>}
                {(this.state.screen === "edit") && <Edit socketRepo={this.props.socketRepo}/>}
            </div>
        );
    }
}

