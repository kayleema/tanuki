import React from 'react';
import './App.css';
import Interact from "./Interact";
import Edit from "./Edit";
import { highlightAllUnder } from 'prismjs';
import GoogleLogin from 'react-google-login';
import GoogleLogout from 'react-google-login';
import {
    BrowserRouter as Router,
    Switch,
    Route,
    Link
} from "react-router-dom";
import replImg from './repl.png';
import favipng from './favipng.png';

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
            <Router>
            <div className="App">
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
                {/* {(this.state.screen === "interact") && <Interact socketRepo={this.props.socketRepo}/>}
                {(this.state.screen === "edit") && <Edit socketRepo={this.props.socketRepo}/>} */}
                <Switch>
                    <Route path="/edit">
                        <Edit socketRepo={this.props.socketRepo}/>
                    </Route>
                    <Route path="/interract">
                        <Interact socketRepo={this.props.socketRepo}/>
                    </Route>
                    <Route path="/">
                        <div className="Home">
                            {/* <div> */}
                                <h1 className="pageTitle">狸語</h1>
                                <h2 className="pageTitle">日本語でプログラミングをしましょう！</h2>
                            {/* </div> */}
                            <div>
                                <h1>GitHubで</h1>
                                <h2>ドキュメンテーションやソースコード</h2>
                                <a href="https://github.com/kayleema/tanuki" className="bigButton">GitHubへ</a>
                                <p>今すぐインストールして使ってみってください。</p>
                            </div>
                            <div>
                                <h1>狸語プログラミングチャレンジしませんか。</h1>
                                <Link to="/edit" className="bigButton">チャレンジへ</Link>
                                <p>
                                    チャレンジしながら<br/>
                                    狸語の使い方を学びましょう<br/>
                                </p>
                                <hr/>
                                <h2>チャレンジのランキング</h2>
                                <ul>
                                    <li>未実装</li>
                                    <li>未実装</li>
                                    <li>未実装</li>
                                </ul>
                            </div>
                            <div>
                                <h1>ライブ実行ＲＥＰＬターミナル。</h1>
                                <Link to="/interract" className="bigButton">ライブ実行へ</Link>
                                <div className="featurePhoto">
                                    <img src={replImg} />
                                </div>
                                <p></p>
                            </div>
                            <div className="footer">
                                ・　狸語　・
                            </div>
                        </div>
                    </Route>
                </Switch>
            </div>
            </Router>
        );
    }
}

