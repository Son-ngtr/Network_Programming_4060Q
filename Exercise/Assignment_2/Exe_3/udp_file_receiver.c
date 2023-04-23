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

    // server addressing
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

    // receive file name
    char filename[MAX_LENGTH];
    memset(filename, 0, MAX_LENGTH);
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int bytes_received = recvfrom(receiver, filename, MAX_LENGTH, 0, (struct sockaddr *)&client_addr, &client_addr_len);
    if (bytes_received < 0)
    {
        perror("recvfrom() failed");
        return 1;
    }
    strcat(filename, ".out");

    // file content 
    char buf[MAX_LENGTH];
    memset(buf, 0, MAX_LENGTH);
    while (1)
    {
        bytes_received = recvfrom(receiver, buf, MAX_LENGTH, 0, (struct sockaddr *)&client_addr, &client_addr_len);
        if(bytes_received < 0){
            perror("recvfrom() failed");
            return 1;
        }
        else if (bytes_received == 0)
        {
            break;
        }
        
        FILE *fp = fopen(filename, "ab");
        if(fp == NULL){
            perror("fopen() failed");
            return 1;
        }
        if(fwrite(buf, 1, bytes_received, fp) != bytes_received){
            perror("fwrite() failed");
            return 1;
        }
        fclose(fp);
    }
    
    // close socket
    close(receiver);

    // inform
    printf("File received from %s:%d successfully!\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    printf("File %s saved successfully!\n", filename);
 
    return 0;
}