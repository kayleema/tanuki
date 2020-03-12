import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import App from './App';
import SocketRepo from './SocketRepo'
import HttpsRedirect from 'react-https-redirect';

let socketRepo = new SocketRepo();

ReactDOM.render(
    <HttpsRedirect><App socketRepo={socketRepo}/></HttpsRedirect>, 
    document.getElementById('root')
);

