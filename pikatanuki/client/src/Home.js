import React from 'react';
import replImg from './repl.png';
import biglogo from './biglogo.png';
import {Link} from "react-router-dom";

export default class Home extends React.Component {
    render() {
        return (
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
                    <h1>GitHubでインストールしませんか？</h1>
                    <h2></h2>
                    <p>ドキュメンテーションやソースコードを見ることができます。</p>
                    <div className="cardFooter">
                        <a href="https://github.com/kayleema/tanuki" className="bigButton">GitHubへ</a>
                    </div>
                </div>
                <div>
                    <h1>狸語プログラミングチャレンジしませんか？</h1>
                    <p>
                        チャレンジしながら、<br/>
                        狸語の使い方を学びましょう。
                    </p>
                    <h2>チャレンジのランキング</h2>
                    <ul>
                        <li>未実装</li>
                        <li>未実装</li>
                        <li>未実装</li>
                    </ul>
                    <div className="cardFooter">
                        <Link to="/edit" className="bigButton">チャレンジへ</Link>
                    </div>
                </div>
                <div>
                    <h1>ライブ実行ＲＥＰＬターミナル</h1>
                    <div className="featurePhoto">
                        <img src={replImg} />
                    </div>
                    <div className="cardFooter">
                        <Link to="/interract" className="bigButton">ライブ実行へ</Link>
                    </div>
                </div>
                <div className="footer">
                    ※　狸語　※
                </div>
            </div>
        );
    }
}