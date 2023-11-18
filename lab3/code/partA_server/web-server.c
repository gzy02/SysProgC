#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUF_SIZE 1024
int is_in_web_root(const char *web_root, const char *file_path)
{
    // 获取file_path的绝对路径
    char *abs_file_path = realpath(file_path, NULL);

    if (abs_file_path == NULL)
    {
        // realpath函数解析失败，无法获取绝对路径
        perror("Failed to get absolute path");
        return 0;
    }

    // 检查abs_file_path是否以web_root开头
    int result = strncmp(web_root, abs_file_path, strlen(web_root)) == 0;
    free(abs_file_path);

    return result;
}
void serve_client(int client_sock, const char *web_root)
{
    char request[BUF_SIZE];
    ssize_t bytes_received = read(client_sock, request, sizeof(request) - 1);
    if (bytes_received < 0)
    {
        perror("read");
        return;
    }
    request[bytes_received] = '\0';

    // Parse the HTTP request, extract the requested file path, and handle the request.
    printf("Received request:\n%s\n", request);

    char method[BUF_SIZE], path[BUF_SIZE];
    int scanResult = sscanf(request, "%s %s", method, path);
    if (scanResult != 2)
    {
        char response[] = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
        write(client_sock, response, strlen(response));
        close(client_sock);
        return;
    }

    char fullpath[BUF_SIZE];
    snprintf(fullpath, sizeof(fullpath), "%s%s", web_root, path);

    // Handle GET requests for serving files.
    if (strcasecmp(method, "GET") == 0)
    {
        // Exercise 5.
        // Add your code here:
        int file_fd = open(fullpath, O_RDONLY);
        char buf[BUF_SIZE];
        char response[BUF_SIZE], response_format[BUF_SIZE];
        if (file_fd < 0 || is_in_web_root(web_root, fullpath) == 0)
        {
            strcpy(buf, "<html><body><h1>404 Not Found</h1></body> </html>");
            strcpy(response_format, "HTTP/1.1 404 Not Found\r\nContent-Length: %d\r\n\r\n%s");
            sprintf(response, response_format, strlen(buf), buf);
        }
        else
        {
            strcpy(response_format, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n%s");
            int read_bytes = read(file_fd, buf, sizeof(buf));
            sprintf(response, response_format, read_bytes, buf);
        }
        write(client_sock, response, strlen(response));
        close(file_fd);
    }

    // Handle DELETE requests for deleting files.
    else if (strcasecmp(method, "DELETE") == 0)
    {
        // Exercise 6: Implement DELETE here
        // Add your code here:
        if (is_in_web_root(web_root, fullpath) == 0 || unlink(fullpath) < 0)
        {
            char response[] = "HTTP/1.1 204 No Content\r\nContent-Length: 0\r\n\r\n";
            write(client_sock, response, strlen(response));
        }
        else
        {
            static char message[] = "<html><body><h1>File deleted.</h1></body></html>";
            char response[BUF_SIZE];
            sprintf(response, "HTTP/1.1 202 Accepted\r\nContent-Length: %ld\r\n\r\n%s", strlen(message), message);
            write(client_sock, response, strlen(response));
        }
    }
    else
    {
        char response[] = "HTTP/1.1 405 Method Not Allowed\r\nContent-Length: 0\r\n\r\n";
        write(client_sock, response, strlen(response));
    }

    close(client_sock);
}

int main()
{
    char web_root[256];
    strcpy(web_root, getenv("HOME"));
    strcat(web_root, "/mywebsite");

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int reuseaddr = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) == -1)
    {
        perror("Setsockopt error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 5) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", SERVER_PORT);

    while (1)
    {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sock < 0)
        {
            perror("accept");
            continue;
        }

        // Challenge: multiple process
        // Add your code here:
        pid_t pid = fork();
        if (pid == 0)
        {
            printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            serve_client(client_sock, web_root); // serve client in child process
            exit(0);
        }
        else if (pid > 0)
        {
            close(client_sock);         // close client socket in parent process
            waitpid(-1, NULL, WNOHANG); // avoid zombie process
        }
        else
        {
            perror("fork");
        }
    }

    close(server_sock);
    return 0;
}
