import React from 'react';
import Editor from 'react-simple-code-editor';
import {highlight, languages} from 'prismjs/components/prism-core';
import 'prismjs/components/prism-clike';
import 'prismjs/components/prism-javascript';
import {tanukiLang} from './languageDef'
import {testFramework} from "./QuestionsRepo"
import "./Edit.css"

export default class Edit extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            resultList: [],
            code: "＃ここで答えを入力してください\n",
            terminalText: "結果はここで開きます。",
            testCode: "",
            selected: 1,
            completed: {},
            showCongrats: false,
            loading: true,
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

    onLogin() {
        // TODO: implement
    }

    success() {
        this.setState({showCongrats: true, loading: true});
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
            this.setState({
                completed: (result.completed === undefined) ? {} : result.completed
            }, () => {this.loadNextQuestion();})
          }
        )
    }

    loadNextQuestion() {
        const done = Object.entries(this.state.completed)
            .filter(entry => entry[1] === true).map(entry => parseInt(entry[0]));
        const lastDone = done.reduce((a, b) => Math.max(a,b), 0);
        console.log('last done', lastDone, "done", done);
        this.handleSelect(lastDone + 1);
    }

    async handleSelect(index) {
        this.setState({loading: true});
        const testText = await fetch(`/questions/test/${index}.pin`).then(res => res.text());
        const codeText = await fetch(`/questions/code/${index}.pin`).then(res => res.text());
        this.setState({
            selected: index,
            testCode: testText,
            code: codeText,
            loading: false,
        })
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

    renderButton(question) {
        const index = question[0];
        const name = question[1];
        const selected = parseInt(question[0]) === parseInt(this.state.selected);
        const finished = this.state.completed[index];
        const locked = !finished && !this.state.completed[index-1] && (index != 1)
        return (
            <button 
                onClick={() => {this.handleSelect(index)}}
                key={index}
                disabled={locked}
                className={selected ? "selected" : ""}
            >
                {this.renderButtonContent(question)}
            </button>
        );
    }

    renderButtonContent(question) {
        const index = question[0];
        const name = question[1];
        const finished = this.state.completed[index];
        const locked = !finished && !this.state.completed[index-1] && (index != 1)
        return (<span>
                {name}
                <span 
                    className={
                        "badge" + 
                        (this.state.completed[index] ? " done" : "") + 
                        (locked ? " lock" : "")
                    }
                >
                {finished && "合格 🎉"}
                {!finished && (this.state.completed[index-1] || index == 1) && "挑戦中 🤔"}
                {locked && "鍵 🗝"}
                </span>
        </span>);
    }

    render() {
        const questions =  this.props.questionsRepo.getQuestions();
        return (
            <div className="editPage">
                {(this.state.showCongrats) && (
                    <div className="loginContainer">
                        <div className="dialog">
                            <div className="hanamaru">💮</div>
                            <hr/>
                            <div className="congrats">
                                <h1>おめでとう</h1>
                                <h2>テスト成功しました</h2>
                                <h1>
                                <a href="#" className="bigButton" onClick={() => {this.setState({showCongrats: false})}}>
                                    次の問題へ進む　→
                                </a>
                                </h1>
                            </div>
                        </div>
                    </div>
                )}
                {this.state.loading && (
                    <div className="loginContainer">
                        <div className="dialog">
                            <div className="lds-hourglass"/>
                            <p>読み込み中…</p>
                        </div>
                    </div>
                )}
                
                <div className="challengeList">
                    <p></p>
                    {Object.entries(questions).map(this.renderButton.bind(this))}
                </div>
                <div className="rhs">
                    <div className="sideBySide">
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
            </div>
        );
    }
}
