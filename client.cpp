#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main() {
    const char path[] = "/tmp/mysock";
    const char clientPath[] = "/tmp/client";
    char *buf = (char *)malloc(40 * sizeof(char));
    if (!buf) {
        perror("Memory allocation failed");
        return EXIT_FAILURE;
    }

    // remove(clientPath);
    int cfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (cfd == -1) {
        perror("Socket creation failed");
        free(buf);
        return EXIT_FAILURE;
    }

    struct sockaddr_un client_address, server_address;
    memset(&client_address, 0, sizeof(client_address));
    memset(&server_address, 0, sizeof(server_address));

    client_address.sun_family = AF_UNIX;
    server_address.sun_family = AF_UNIX;

    strncpy(&server_address.sun_path[1], path, sizeof(server_address.sun_path) - 1);
    server_address.sun_path[sizeof(server_address.sun_path) - 1] = '\0';

    strncpy(&client_address.sun_path[1], clientPath, sizeof(client_address.sun_path) - 1);
    client_address.sun_path[sizeof(client_address.sun_path) - 1] = '\0';

    if (bind(cfd, (struct sockaddr*)&client_address, sizeof(client_address)) == -1) {
        perror("Bind failed");
        free(buf);
        close(cfd);
        return EXIT_FAILURE;
    }

    if (connect(cfd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Connect failed");
        free(buf);
        close(cfd);
        return EXIT_FAILURE;
    }

    int readStatus;
    while ((readStatus = read(STDIN_FILENO, buf, 40)) > 0) {
        if (write(cfd, buf, readStatus) != readStatus) {
            perror("Partial read/write");
            break;
        }
    }

    if (readStatus == -1) {
        perror("Read error");
    }

    free(buf);
    close(cfd);
    return EXIT_SUCCESS;
}
