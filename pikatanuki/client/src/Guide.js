import React from 'react';
import ReactMarkdown from 'react-markdown';
import toc from 'remark-toc';
import readme from './reference.md'
import './guide.css';

function flatten(text, child) {
    return typeof child === 'string'
      ? text + child
      : React.Children.toArray(child.props.children).reduce(flatten, text)
}

function HeadingRenderer(props) {
    var children = React.Children.toArray(props.children)
    var text = children.reduce(flatten, '')
    var slug = text.toLowerCase() //text.toLowerCase().replace(/\W/g, '-')
    return React.createElement('h' + props.level, {id: slug}, props.children)
}

function DefinitionRenderer(props) {
    console.log(props);
    return React.createElement('dd', {}, props.children)
}

export default class Guide extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            readme: "",
        }
    }

    componentDidMount() {
        fetch(readme).then(res=> res.text()).then(text => {
            this.setState({readme: text})
        })
    }

    render() {
        return (
            <div className="guideBackground">
            <div className="guide">
                <ReactMarkdown 
                    source={this.state.readme} 
                    plugins={[toc]}
                    renderers={{
                        heading: HeadingRenderer,
                        definition: DefinitionRenderer,
                    }}
                />
            </div>
            </div>
        )
    }
}