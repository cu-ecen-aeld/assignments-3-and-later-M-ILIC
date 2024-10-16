#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdbool.h>

int main() {
    int socket_fd;
    struct addrinfo hints;
    struct addrinfo *serverinfo;
    struct sockaddr_in serveraddr;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    int status = getaddrinfo("localhost", "9000", &hints, &serverinfo);
    if (status != 0){
        printf("ERROR: getaddrinfo()");
        return 1;
    }
    memcpy(&serveraddr, serverinfo->ai_addr, sizeof(serveraddr));
    if (connect(socket_fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
        printf("ERROR: connect()");
        return 1;
    }
    char buffer[1000];
    char msg1[6] = {'h', 'e', 'r', 'r', 'o', '\n'};
    status = send(socket_fd, msg1, sizeof(msg1), 0);
    printf("MESSAGE SENT, status: %i\n", status);
    status = recv(socket_fd, buffer, sizeof(buffer), 0);
    printf("Recieved this bytes: %i\n", status);
    buffer[status] = '\0';
    printf("%s\n", buffer);
    return 0;
}