import React from 'react';
import './App.css';
import Interact from "./Interact";
import Edit from "./Edit";
import Home from "./Home";
import GoogleLogin from 'react-google-login';
import {
    BrowserRouter as Router,
    Switch,
    Route} from "react-router-dom";
import QuestionsRepo from './QuestionsRepo';

export default class App extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            screen: "edit",
            googleToken: localStorage.getItem("googleToken"),
            googleEmail: localStorage.getItem("googleEmail"),
            googleExpire: localStorage.getItem("googleExpire")
        }
    }

    componentDidMount() {
        this.props.socketRepo.startSocket();
        this.props.socketRepo.setOnClose(this.socketClose.bind(this));
        this.props.socketRepo.setOnOpen(this.socketOpen.bind(this));
        var timestamp = (new Date()).getTime();
        if (this.state.googleExpire <= Date.now() || (this.state.googleEmail == undefined) || (this.state.googleToken == undefined)) {
            this.logout();
        } else {
            console.log("login session remaining", (this.state.googleExpire - timestamp) / 1000 / 60, "min");
            this.timeoutNotificationSetup();
        }
    }

    timeoutNotificationSetup() {
        var timestamp = (new Date()).getTime();
        const remaining = (this.state.googleExpire - timestamp);
        console.log('timeout notification in', remaining/1000/60, "min")
        setTimeout(() => {
            this.logout();
        }, remaining)
    }

    socketOpen() {
        this.setState({socketState: "open"});
    }

    socketError(e) {
        console.log("socket error", e);
    }

    socketClose() {
        this.setState({socketState: "close"});
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
        localStorage.setItem("googleExpire", event.tokenObj.expires_at)
        this.setState({
            googleToken: event.tokenId,
            googleEmail: event.profileObj.email,
            googleExpire: event.tokenObj.expires_at,
        }, () => {
            this.timeoutNotificationSetup();
            this.props.loginRepository.login();
        })
    }

    logout(event) {
        localStorage.removeItem("googleToken");
        localStorage.removeItem("googleEmail");
        localStorage.removeItem("googleExpire");
        this.setState(
            {
                googleToken: undefined,
                googleEmail: undefined,
                googleExpire: undefined,
            }
        )
    }

    render() {
        return (
            <Router>
            <div className="App">
                {this.state.socketState != "open" && (
                    <div className="loginContainer">
                        <div className="dialog">
                            <div className="lds-hourglass"/>
                            <p>ウエッブソケット読み込み中</p>
                        </div>
                    </div>
                )}
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
                            <h1>🦝・🦝・🦝</h1>
                        </div>
                    </div>
                )}
                <div className="header">
                    <a className="logo" href="/">狸語</a>
                    {process.env.NODE_ENV == "development" && <em>　　　　　開発環境</em>}
                    <span className="accountHeader">
                    {this.state.googleToken != undefined && (
                        <span className="loginMessage">"{this.state.googleEmail}"としてログインされました</span>
                    )}
                    {this.state.googleToken != undefined && (
                        <button onClick={this.logout.bind(this)}>
                            ログアウト
                        </button>
                    )}
                    </span>
                </div>
                <Switch>
                    <Route path="/edit">
                        <Edit
                            socketRepo={this.props.socketRepo}
                            questionsRepo={new QuestionsRepo()}
                        />
                    </Route>
                    <Route path="/interract">
                        <Interact
                            socketRepo={this.props.socketRepo}
                            loginRepository={this.props.loginRepository}
                        />
                    </Route>
                    <Route path="/">
                        <Home/>
                    </Route>
                </Switch>
            </div>
            </Router>
        );
    }
}

