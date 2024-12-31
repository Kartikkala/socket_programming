#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main()
{
    const char path[] = "/tmp/mysock";
    const char clientPath[] = "/tmp/client";
    char *buf = (char*)malloc(sizeof(char) * 40);
    int readStatus = 0;
    remove(clientPath);
    int cfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(cfd == -1)
        perror("Socket:");
    
    struct sockaddr_un client_address;
    struct sockaddr_un server_address;
    memset(&client_address, 0, sizeof(client_address));
    memset(&server_address, 0, sizeof(server_address));

    server_address.sun_family = AF_UNIX;
    client_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, path, sizeof(path));
    strncpy(client_address.sun_path, clientPath, sizeof(clientPath));
    if(bind(cfd, (struct sockaddr*)&client_address, sizeof(struct sockaddr_un)) == -1)
        perror("Bind:");
    if(connect(cfd, (struct sockaddr*)&server_address, sizeof(struct sockaddr_un)) == -1)
    perror("Connect: ");


    while((readStatus = read(STDIN_FILENO, (char*)buf, 40)) > 0)
    {
        if(write(cfd, buf, 40) != readStatus)
            perror("Partial read/write");
        if(readStatus == -1)
            perror("Read error!");
    }
    exit(EXIT_SUCCESS);
}