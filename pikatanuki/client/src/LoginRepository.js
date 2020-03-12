
export default class LoginRepository {
    setLoginListener(handler) {
        this.loginListener = handler;
    }

    login() {
        this.loginListener && this.loginListener();
    }
}
