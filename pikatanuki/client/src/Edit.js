import React from 'react';
import Editor from 'react-simple-code-editor';
import {highlight, languages} from 'prismjs/components/prism-core';
import 'prismjs/components/prism-clike';
import 'prismjs/components/prism-javascript';
import {tanukiLang} from './languageDef'

const mainTestCode = `\
＃＃＃
＃＃＃　第一　文字列
＃＃＃　　回文（palindrome）かどうかを確認する関数を作る
＃＃＃　　（Hint：逆文字列という関数はあります）
＃＃＃

関数、試験一覧・回文（）
　確認、回文ですか（「こんにちは」）＝＝０
　確認、回文ですか（「よく行くよ」）＝＝１

全試験実行（）
`

const testFramework = `
関数、試験実行（試験関数、名前）
　表示（「＊＊＊　試験実行：」、名前、「　＊＊＊」）
　結果＝試験関数（）
　もし、結果＝＝１
　　返す、１
　表示（「＊＊＊　……合格」）

関数、全試験実行（）
　全部結果＝１
　表示（「全試験始まります」）
　関数、ループ（試験名、試験関数）
　　外側、全部結果
　　部分結果＝試験実行（試験関数、試験名）
　　もし、部分結果＝＝１
　　　全部結果＝０
　　返す、部分結果
　それぞれ（試験一覧、ループ）
　もし、全部結果＝＝１
　　表示（「✨試験全体は成功✨」）

関数、確認イコール（左、右）
　もし、左＝＝右
　　返す、１
　その他
　　表示（「確認エラー。左：」、左、「、右：」、右）
　　確認、０

＃全テストが入っている辞書
試験一覧＝辞書（）
`

/*
関数、回文ですか（入力）
　返す、逆文字列（入力）＝＝入力
*/

const questions = {
    1 :  "問題一：回文",
}

export default class Edit extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            resultList: [],
            code: "関数、回文ですか（入力）\n　返す、０\n",
            terminalText: "結果はここで開きます。",
            testCode: mainTestCode,
            selected: 1,
            completed: {},
        };
    }

    onSocketMessage(data) {
        console.log(data);
        this.setState(state => ({
            resultList: [...state.resultList, data],
        }))
        if (data.messageType == "display" && data.message == "✨試験全体は成功✨") {
            console.log('successsssss');
            this.success();
        } 
    }

    success() {
        fetch("https://tanukisekai.kaylee.jp/profile/complete/" + this.state.selected, {
            method: "post",
            headers: new Headers({
              'Authorization': 'Bearer ' + localStorage.getItem('googleToken')
            }),
        })
        .then(res => {
            this.fetchProfile();
        })
    }

    fetchProfile() {
        fetch("https://tanukisekai.kaylee.jp/profile", {
            headers: new Headers({
              'Authorization': 'Bearer ' + localStorage.getItem('googleToken')
            }),
        })
        .then(res => res.json())
        .then(
          (result) => {
            console.log(result);
            this.setState({completed: (result.completed === undefined) ? {} : result.completed})
          }
        )
    }

    componentDidMount() {
        this.props.socketRepo.setOnMessage(this.onSocketMessage.bind(this));
        this.fetchProfile();
    }

    executeCode() {
        this.setState(state => ({
            resultList: [],
        }))
        this.props.socketRepo.sendFile(this.state.code + testFramework + this.state.testCode);
    }

    render() {
        return (
            <div className="editPage">
                <div className="challengeList">
                    {this.state.expanded != true && (
                        <button 
                            onClick={() => {this.setState({expanded:true})}}
                        >
                            {questions[this.state.selected]}
                            <span className={"badge" + (this.state.completed[this.state.selected] ? " done" : "")}>
                            {this.state.completed[this.state.selected] && "完成"}
                            {!this.state.completed[this.state.selected] && "未完成"}
                            </span>
                        </button>
                    )}
                    {this.state.expanded == true && [
                        Object.entries(questions).map((question) => (
                            <button 
                                    onClick={() => {this.setState({expanded:false, selected: question[0]})}} 
                            >
                                {question[1]}
                                <span className={"badge" + (this.state.completed[question[0]] ? " done" : "")}>
                                {this.state.completed[question[0]] && "🎉完成"}
                                {!this.state.completed[question[0]] && "未完成"}
                                </span>
                            </button>
                        )),
                        <p><em>他の問題はまだ開発中です、すみません。</em></p>
                    ]}
                    {this.state.expanded == true && (<p></p>)}
                </div>
                <div className="sideBySide">
                    <div className="outArea outAreaEditor">
                        <div>
                            <Editor
                                value={this.state.code}
                                onValueChange={code => this.setState({code})}
                                highlight={(code) => (
                                    highlight(code, tanukiLang)
                                )}
                                padding={10}
                                style={{
                                    fontFamily: '"Fira code", "Fira Mono", monospace',
                                    fontSize: 16,
                                    outline: "none",
                                }}
                            />
                        </div>
                    </div>
                    <div className="outArea outAreaEditor outAreaTest">
                        <div style={{width: "1000px"}}>
                            <Editor
                                value={this.state.testCode}
                                highlight={(code) => (
                                    highlight(code, tanukiLang)
                                )}
                                readOnly={true}
                                padding={10}
                                style={{
                                    fontFamily: '"Fira code", "Fira Mono", monospace',
                                    fontSize: 16,
                                    outline: "none",
                                }}
                            />
                        </div>
                    </div>
                </div>
                <div className="header">
                    <button onClick={this.executeCode.bind(this)}>🏃実行する</button>
                </div>
                <div className="terminal">
                    <pre>
                    {this.state.resultList.map((item, i) => {
                        if (item.messageType == "display") {
                            return (<span key={i}>{item.message}</span>)
                        }
                    })}
                    </pre>
                </div>
            </div>
        );
    }
}
