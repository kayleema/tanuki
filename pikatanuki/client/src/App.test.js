import React from 'react';
import ReactDOM from 'react-dom';
import App from './App';
import * as sinon from 'sinon';
import SocketRepo from "./SocketRepo";

describe('REPL Loop', function () {
    it('renders without crashing', () => {
        const div = document.createElement('div');
        const socketRepo = sinon.createStubInstance(SocketRepo);
        ReactDOM.render(<App socketRepo={socketRepo}/>, div);

        expect(socketRepo.startSocket.calledOnce).toBe(true);
        ReactDOM.unmountComponentAtNode(div);
    });
});
