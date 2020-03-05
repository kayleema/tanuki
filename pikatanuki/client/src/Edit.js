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
            textContent: [],
            code: "",
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

    onSocketMessage(data) {
        this.setState(state => ({
            textContent: [...state.textContent, data],
        }))
    }

    componentDidMount() {
        this.props.socketRepo.startSocket();
        this.props.socketRepo.setOnOpen(this.onSocketOpen.bind(this));
        this.props.socketRepo.setOnMessage(this.onSocketMessage.bind(this));
        this.props.socketRepo.setOnClose(this.onSocketClose.bind(this));
    }

    executeCommand() {
    }

    render() {
        return (
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
                            fontSize: 14,
                            outline: "none",
                        }}
                    />
                </div>
            </div>
        );
    }
}
