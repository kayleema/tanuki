import React from 'react';
import ReactDOM from 'react-dom';
import App from './App';
import SocketRepo from "./SocketRepo";
import ReactTestUtils from 'react-dom/test-utils';
import Interact from "./Interact";

jest.mock('./SocketRepo')

describe('REPL Loop', () => {
    let socketRepo, container;
    beforeEach(() => {
        container = document.createElement('div');

        SocketRepo.mockClear();
        socketRepo = new SocketRepo();

        ReactDOM.render(<Interact socketRepo={socketRepo}/>, container);
    });

    test('renders connection message', () => {
        expect(socketRepo.startSocket).toHaveBeenCalledTimes(1);
        expect(container.querySelector('.statusArea').textContent).not.toContain('接続した');
        socketRepo.setOnOpen.mock.calls[0][0]();
        expect(container.querySelector('.statusArea').textContent).toContain('接続した');
    });

    test('renders sent code', () => {
        const textarea = container.querySelector("textarea");
        ReactTestUtils.Simulate.change(textarea, {target: {value: "source code"}});
        ReactTestUtils.Simulate.keyDown(textarea, {key: "Enter", keyCode: 13, which: 13});
        expect(container.querySelector('.outAreaItemInput').textContent).toContain('source code');

        expect(socketRepo.sendCode).toBeCalledWith("source code");
    });

    test('renders received result', () => {
        const onMessageHandler = socketRepo.setOnMessage.mock.calls[0][0];
        onMessageHandler({messageType: "result", resultString: "resultstring"});
        expect(container.querySelector('.outArea').textContent).toContain('resultstring');
    });

    test('renders received display', () => {
        const onMessageHandler = socketRepo.setOnMessage.mock.calls[0][0];
        onMessageHandler({messageType: "display", message: "resultstring"});
        expect(container.querySelector('.outArea').textContent).toContain('resultstring');
    });

    afterEach(() => {
        ReactDOM.unmountComponentAtNode(container);
    })
});
