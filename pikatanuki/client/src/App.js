import React from 'react';
import './App.css';
import Interact from "./Interact";
import Edit from "./Edit";
import GoogleLogin from 'react-google-login';
import GoogleLogout from 'react-google-login';
import {
    BrowserRouter as Router,
    Switch,
    Route,
    Link
} from "react-router-dom";
import replImg from './repl.png';
import biglogo from './biglogo.png';
import {tanukiLang} from './languageDef'
import {highlight, languages} from 'prismjs/components/prism-core';

const code_example = `\
＃例文
関数、フィボ（号）
　　もし、号＝＝１
　　　返す、１
　　もし、号＝＝０
　　　返す、１
　　あ＝フィボ（号－１）
　　い＝フィボ（号－２）
　　返す、あ＋い
`;

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
        // this.props.socketRepo.setOnError(this.socketError.bind(this));
        var timestamp = (new Date()).getTime();
        if (this.state.googleExpire <= Date.now()) {
            this.logout();
        } else {
            console.log("login session remaining ", (this.state.googleExpire - timestamp) / 1000 / 60, "min");
        }
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
                            <img src={biglogo} className="biglogo"/>
                            <h1 className="pageTitle">狸語</h1>
                            <h2 className="pageTitle">日本語でプログラミングをしよう<span className="jump">！</span></h2>
                            <hr/>
                            <div className="plain fade-in"><br/><br/><br/><br/>
                                    <span className="token comment">＃例文</span><br/>
                                    <span className="token keyword">関数</span><span className="token punctuation">、</span><span className="token function">フィボ</span><span className="token punctuation">（</span>号<span className="token punctuation">）</span><br/>
                                    　<span className="token keyword">もし</span><span className="token punctuation">、</span>号<span className="token operator">＝</span><span className="token operator">＝</span><span className="token number">１</span><br/>
                                    　　<span className="token keyword">返す</span><span className="token punctuation">、</span><span className="token number">１</span><br/>
                                    　<span className="token keyword">もし</span><span className="token punctuation">、</span>号<span className="token operator">＝</span><span className="token operator">＝</span><span className="token number">０</span><br/>
                                    　　<span className="token keyword">返す</span><span className="token punctuation">、</span><span className="token number">１</span><br/>
                                    　あ<span className="token operator">＝</span>フィボ<span className="token punctuation">（</span>号<span className="token number">－１</span><span className="token punctuation">）</span><br/>
                                    　い<span className="token operator">＝</span>フィボ<span className="token punctuation">（</span>号<span className="token number">－２</span><span className="token punctuation">）</span><br/>
                                    　<span className="token keyword">返す</span><span className="token punctuation">、</span>あ<span className="token operator">＋</span>い<br/>
                            </div>
                            <div>
                                <h1>GitHubで</h1>
                                <h2>ドキュメンテーションやソースコード</h2>
                                <a href="https://github.com/kayleema/tanuki" className="bigButton">GitHubへ</a>
                                <p>今すぐインストールして使てみってください。</p>
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
                                ※　狸語　※
                            </div>
                        </div>
                    </Route>
                </Switch>
            </div>
            </Router>
        );
    }
}

