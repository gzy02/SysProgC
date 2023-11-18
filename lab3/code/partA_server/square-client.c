#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define BUF_SIZE 1024

int main()
{
    int client_sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE], buffer_recv[BUF_SIZE];

    client_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_sock_fd == -1)
    {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(client_sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server. Enter 'bye' to exit.\n");

    int message_id = 0;
    struct timeval MYTIMEOUT = {.tv_sec = 3, .tv_usec = 0};
    setsockopt(client_sock_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&MYTIMEOUT, sizeof(struct timeval));
    while (1)
    {
        printf("Enter a message: ");
        sprintf(buffer, "%d ", message_id);
        int offset = strlen(buffer);
        fgets(buffer + offset, BUF_SIZE - offset, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer + offset, "bye") == 0)
        {
            printf("Exiting...\n");
            break;
        }

        ssize_t received_bytes, sent_bytes;
        while (1)
        {
            sent_bytes = write(client_sock_fd, buffer, strlen(buffer));
            received_bytes = read(client_sock_fd, buffer_recv, sizeof(buffer));
            if (received_bytes > 0)
            {
                buffer_recv[received_bytes] = '\0';
                printf("Server response: %s\n", buffer_recv);
                if (atoi(buffer_recv) == message_id)
                    break;
            }
        }
        printf("Answer = %s\n", buffer_recv + strcspn(buffer_recv, " ") + 1);
        message_id++;
    }

    close(client_sock_fd);

    return 0;
}
