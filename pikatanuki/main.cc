#include <signal.h>

#include "TanukiServer.h"

TanukiServer *running_server = nullptr;

void shutdownInterrupt(int) {
    running_server->stop();
}

int main() {
    TanukiServer s;
    running_server = &s;
    signal(SIGINT, shutdownInterrupt);
    signal(SIGTERM, shutdownInterrupt);
    s.run();
    return 0;
}
