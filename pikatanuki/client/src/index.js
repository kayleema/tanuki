import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import App from './App';
import SocketRepo from './SocketRepo'

let socketRepo = new SocketRepo();

ReactDOM.render(<App socketRepo={socketRepo}/>, document.getElementById('root'));

