import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import App from './App';
import SocketRepo from './SocketRepo'
import HttpsRedirect from 'react-https-redirect';
import LoginRepository from './LoginRepository';

let socketRepo = new SocketRepo();
let loginRepo = new LoginRepository();

ReactDOM.render(
    <HttpsRedirect>
        <App
            socketRepo={socketRepo}
            loginRepository={loginRepo}
        />
    </HttpsRedirect>, 
    document.getElementById('root')
);

