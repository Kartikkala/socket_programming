#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>


int main()
{
    char *buf = (char*)malloc(sizeof(char) * 40);
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    int cfd = 0;
    int readStatus = 0;
    char socketPath[] = "/tmp/mysock";
    remove(socketPath);
    printf("%d", remove(socketPath));
    if(sfd == -1)
        perror("socket");
    struct sockaddr_un addr;

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath, sizeof(addr.sun_path)-1);

    if(bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
        perror("bind");

    if(listen(sfd, 20) == -1)
        perror("listen");

    while(true)
    {
        cfd = accept(sfd, NULL, 0);
        if(cfd == -1)
            perror("accept");
        while((readStatus = read(cfd, (char *)buf, 40)) > 0)
        {
            if(write(STDOUT_FILENO, buf, 40) != readStatus)
                perror("Partial read/write");
            if(readStatus == -1)
                perror("Read error from client socket!");
            if(close(cfd) == -1)
                perror("Close");
        }
    }
    exit(EXIT_SUCCESS);
}
