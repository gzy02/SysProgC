#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define BUF_SIZE 1024

int main(){
    int client_sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE];

    client_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_sock_fd == -1){
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if(inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0){
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    if(connect(client_sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server. Enter 'bye' to exit.\n");

    while(1){
        printf("Enter a message: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';  // Remove trailing newline character

        if(write(client_sock_fd, buffer, strlen(buffer)) == -1){
            perror("Write error");
            break;
        }

        if(strcmp(buffer, "bye") == 0){
            printf("Exiting...\n");
            break;
        }

        ssize_t received_bytes = read(client_sock_fd, buffer, sizeof(buffer));
        if(received_bytes <= 0){
            perror("Receive error");
            break;
        }

        buffer[received_bytes] = '\0';
        printf("Server response: %s\n", buffer);
    }

    close(client_sock_fd);

    return 0;
}
