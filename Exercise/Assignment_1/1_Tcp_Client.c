#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024

int main(int argc, char *argv[])
{
    // Check đầu vào
    if (argc != 3)
    {
        printf("Usage: %s <server-IP-address> <Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Thông tin địa chỉ Socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    // Tạo Socket
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client == -1)
    {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    // Kết nối Server
    if (connect(client, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("connect() failed");
        exit(EXIT_FAILURE);
    }
    printf("Connection to %s %s port [tcp/*] succeeded!\n", argv[1], argv[2]);

    // Gửi dữ liệu đến Server từ bàn phím
    char buf[MAX_BUF_SIZE];
    memset(buf, 0, MAX_BUF_SIZE);
    while (1)
    {
        printf("Enter a message: ");
        fgets(buf, MAX_BUF_SIZE, stdin);

        if (send(client, buf, strlen(buf), 0) == -1)
        {
            perror("send() failed");
            exit(EXIT_FAILURE);
        }

        if (strcmp(buf, "exit\n") == 0)
        {
            break;
        }
    }
    // Đóng socket
    close(client);

    return 0;
}