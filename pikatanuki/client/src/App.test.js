import React from 'react';
import ReactDOM from 'react-dom';
import App from './App';
import SocketRepo from "./SocketRepo";
jest.mock('./SocketRepo')

describe('REPL Loop',  () => {
    beforeEach(() => {
        SocketRepo.mockClear();
    });

    test('renders without crashing', () => {
        const div = document.createElement('div');
        const socketRepo = new SocketRepo();
        ReactDOM.render(<App socketRepo={socketRepo}/>, div);

        expect(socketRepo.startSocket).toHaveBeenCalledTimes(1);
        ReactDOM.unmountComponentAtNode(div);
    });
});
