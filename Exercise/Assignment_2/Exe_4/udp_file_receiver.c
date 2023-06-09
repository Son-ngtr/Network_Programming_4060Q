#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_LENGTH 1024

int main(int argc, char *argv[])
{
    // input checking
    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    // socket creation
    int receiver = socket(AF_INET, SOCK_DGRAM, 0);
    if (receiver < 0)
    {
        perror("socket() failed");
        return 1;
    }

    // server receiver addressing
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    // socket address
    if (bind(receiver, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind() failed");
        return 1;
    }

    // content receiving
    char buf[MAX_LENGTH];
    memset(buf, 0, MAX_LENGTH);
    int bytes_received = 0;
    while ((bytes_received = recvfrom(receiver, buf, MAX_LENGTH, 0, NULL, NULL)) > 0)
    {
        printf("Received %d bytes: %s\n", bytes_received, buf);
        char *pos = strchr(buf, '-');
        char filename[31];
        memset(filename, 0, 31);
        strncpy(filename, buf, pos - buf);
        FILE *fp = fopen(filename, "ab");
        if (fp == NULL)
        {
            perror("fopen() failed");
            return 1;
        }
        fwrite(pos + 1, 1, strlen(pos + 1), fp);
        fclose(fp);
        memset(buf, 0, MAX_LENGTH);
    }

    // close socket
    close(receiver);

    return 0;
}