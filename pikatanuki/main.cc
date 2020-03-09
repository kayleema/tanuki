#include <signal.h>

#include "TanukiServerREPL.h"

TanukiServerREPL *running_server = nullptr;

void shutdownInterrupt(int) {
    running_server->stop();
}

int main() {
    TanukiServerREPL s;
    running_server = &s;
    signal(SIGINT, shutdownInterrupt);
    signal(SIGTERM, shutdownInterrupt);
    s.run();
    return 0;
}
