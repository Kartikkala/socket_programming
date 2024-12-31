#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <cerrno>

int main() {
    const char socketPath[] = "/tmp/mysock";
    char *buf = new char[40];
    if (remove(socketPath) == -1 && errno != ENOENT) {
        perror("remove");
        delete[] buf;
        return EXIT_FAILURE;
    }

    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        delete[] buf;
        return EXIT_FAILURE;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath, sizeof(addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        close(sfd);
        delete[] buf;
        return EXIT_FAILURE;
    }

    if (listen(sfd, 20) == -1) {
        perror("listen");
        close(sfd);
        delete[] buf;
        return EXIT_FAILURE;
    }

    while (true) {
        int cfd = accept(sfd, nullptr, nullptr);
        if (cfd == -1) {
            perror("accept");
            continue;
        }

        int readStatus;
        while ((readStatus = read(cfd, buf, 40)) > 0) {
            if (write(STDOUT_FILENO, buf, readStatus) != readStatus) {
                perror("write");
                break;
            }
        }

        if (readStatus == -1) {
            perror("read");
        }

        if (close(cfd) == -1) {
            perror("close client socket");
        }
    }

    close(sfd);
    delete[] buf;
    return EXIT_SUCCESS;
}
