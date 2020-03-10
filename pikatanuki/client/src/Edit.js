import React from 'react';
import Editor from 'react-simple-code-editor';
import {highlight, languages} from 'prismjs/components/prism-core';
import 'prismjs/components/prism-clike';
import 'prismjs/components/prism-javascript';

Prism.languages.tanuki = {
    'function': [
        {
            lookbehind: true,
            pattern: /(関数、)[^（]*(?=（)/,
            greedy: true
        },
    ],
    'comment': /＃.*/,
    'string': /「[^」]*」/,
    'builtin': /表示|配列|辞書|試験一覧|自分|確認|それぞれ/,
    'number': /－?[１２３４５６７８９０]+/,
    'operator': /[＋－＊／・〜＝＞＜]/,
    'punctuation': /[（）【】、]/,
    'keyword': /関数|もし|あるいは|その他|返す/,
};

export default class Edit extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            resultList: [],
            code: "表示（「こんにちは」）",
            terminalText: "結果はここで開きます。",
        };
    }

    onSocketMessage(data) {
        console.log(data);
        this.setState(state => ({
            resultList: [...state.resultList, data],
        }))
    }

    componentDidMount() {
        this.props.socketRepo.setOnMessage(this.onSocketMessage.bind(this));
    }

    executeCode() {
        this.setState(state => ({
            resultList: [],
        }))
        this.props.socketRepo.sendFile(this.state.code);
    }

    render() {
        return (
            <>
                <div className="outArea outAreaEditor">
                    <div>
                        <Editor
                            value={this.state.code}
                            onValueChange={code => this.setState({code})}
                            highlight={(code) => (
                                highlight(code, languages.tanuki)
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
                <div className="header">
                    <button onClick={this.executeCode.bind(this)}>🏃実行する</button>
                </div>
                <div className="terminal">
                    {this.state.resultList.map((item) => {
                        if (item.messageType == "display") {
                            return (<div>{item.message}</div>)
                        }
                    })}
                </div>
            </>
        );
    }
}
