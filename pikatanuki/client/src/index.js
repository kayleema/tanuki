import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import App from './App';
import SocketRepo from './SocketRepo'
import HttpsRedirect from 'react-https-redirect';
import LoginRepository from './LoginRepository';
import '../questions/code/1.pin'
import '../questions/test/1.pin'
import '../questions/code/2.pin'
import '../questions/test/2.pin'
import '../questions/code/3.pin'
import '../questions/test/3.pin'
import '../questions/code/4.pin'
import '../questions/test/4.pin'
import '../questions/code/5.pin'
import '../questions/test/5.pin'
import '../questions/code/6.pin'
import '../questions/test/6.pin'
import '../questions/code/7.pin'
import '../questions/test/7.pin'

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

