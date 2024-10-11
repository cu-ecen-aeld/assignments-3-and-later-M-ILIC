#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>

bool caught_sigint = false;
bool caught_sigterm = false;
char *filename = "/var/tmp/aesdsocketdata";

int return_failure(char *error_str, struct addrinfo *server_info){
    printf(error_str);
    printf("\n");
    freeaddrinfo(server_info);
    remove(filename);
    closelog();
    return -1;
}


int receive_message(struct addrinfo *server_info, int new_fd, char *client_ip_str){
    int status;
    FILE *file;
    char buffer[1000];
    while((status = recv(new_fd, buffer, sizeof(buffer), 0)) == sizeof(buffer) && buffer[status-1] != '\n'){
        // printf("Recieved this bytes: %i\n", status);
        file = fopen(filename, "a"); 
        status = fwrite(buffer, 1, status, file);
        if (status == -1){
            return_failure("ERROR: writing to file failed", server_info);
        }
        status = fclose(file);
        if (status == -1){
            return_failure("ERROR: closing file failed", server_info);
        }
    }
    if (status > 0){
        buffer[status] = '\0';
        // printf("Recieved this bytes: %i\n", status);
        // printf("%s\n", buffer);
        file = fopen(filename, "a"); 
        status = fwrite(buffer, 1, status, file);
        if (status == -1){
            return_failure("ERROR: writing to file failed", server_info);
        }
        status = fclose(file);
        if (status == -1){
            return_failure("ERROR: closing file failed", server_info);
        }
    } else if (status == -1){
        return_failure("ERROR: receiving to buffer failed", server_info);
    }
    return 0;   
}

int send_file_to_client(struct addrinfo *server_info, int new_fd){
    char buffer[1000];
    int status, prev_status;
    FILE *file;
    file = fopen(filename, "r");
    while((status = fread(buffer, 1, sizeof(buffer), file)) > 0){
        // printf("Sending this bytes: %i\n", status);
        prev_status = status;
        status = send(new_fd, buffer, status, 0);
        if (status == -1){
            return_failure("ERROR: sending file to client failed", server_info);
        }
    }
    if (status == -1){
        return_failure("ERROR: reading file failed", server_info);
    }
    status = fclose(file);
    if (status == -1){
        return_failure("ERROR: closing file failed", server_info);
    }
    return 0;
}

void signal_handler(int signal_number){
    if (signal_number == SIGINT){
        printf("CAUGHT SIGINT, will now gracefully terminate\n");
        syslog(LOG_INFO ,"Caught signal, exiting");
        caught_sigint = true;
    } else if ( signal_number == SIGTERM ){
        printf("CAUGHT SIGTERM, will now gracefully terminate\n");
        syslog(LOG_INFO ,"Caught signal, exiting");
        caught_sigterm = true;
    }
}

int main(int argc, char *argv[]) {
    char *SERVER_PORT = "9000";
    char *HOSTNAME = "localhost";
    int status, socket_fd, new_fd;
    struct addrinfo hints;
    struct addrinfo *server_info;
    struct sockaddr *client;
    socklen_t addr_size;
    char client_ip_str[20];
    char log_accepted_str[50] = "Accepted connection from ";
    char log_closed_str[50] = "Closed connection from ";
    struct sigaction new_action;
    pid_t pid;

    memset(&new_action, 0, sizeof(struct sigaction));
    new_action.sa_handler = signal_handler;
    sigaction(SIGTERM, &new_action, NULL);
    sigaction(SIGINT, &new_action, NULL);

    openlog(NULL, 0, LOG_USER);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    socket_fd = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
    status = getaddrinfo(HOSTNAME, SERVER_PORT, &hints, &server_info);
    if (status != 0){
        printf("ERROR: getaddrinfo failed");
        closelog();
        return -1;
    }
    status = bind(socket_fd, server_info->ai_addr, server_info->ai_addrlen);
    if (status != 0){
        return_failure("ERROR: bind failed", server_info);
    }
    if( argc == 2 && strcmp(argv[1], "-d") == 0){ //daemon mode
        pid = fork();
        if(pid != 0){
            exit(0);
        }
    }
    while(true){
        status = listen(socket_fd, 10);
        if (status != 0){
            if (caught_sigint || caught_sigterm){
                break;
            }
            printf("ERROR: listen failed\n");
            continue;
        }
        new_fd = accept(socket_fd,(struct sockaddr*)&client, &addr_size);
        if (new_fd < 0){
            if (caught_sigint || caught_sigterm){
                break;
            }
            printf("ERROR: accept failed\n");
            continue;
        }
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)&client;
        inet_ntop(AF_INET, &(ipv4->sin_addr), client_ip_str, sizeof(client_ip_str));
        log_accepted_str[25] = '\0'; // to remove previous client ip
        strcat(log_accepted_str, client_ip_str);
        printf("%s\n", log_accepted_str);
        syslog(LOG_INFO, log_accepted_str);
        if (receive_message(server_info, new_fd, client_ip_str) != 0){
            continue;
        }
        if (send_file_to_client(server_info, new_fd) != 0){
            continue;
        }
        log_closed_str[23] = '\0';
        strcat(log_closed_str, client_ip_str);
        syslog(LOG_INFO, log_closed_str);
        if (caught_sigint || caught_sigterm){
            break;
        }
    }
    freeaddrinfo(server_info);
    remove(filename);
    closelog();
    return 0;
}