#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#define MAX_CLIENT 10

int main(int argc, char *argv[])
{
    // input checking
    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // socket address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    // socket creation
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1)
    {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    // socket addressing
    if (bind(server, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    // listening
    if (listen(server, MAX_CLIENT) == -1)
    {
        perror("listen() failed");
        exit(EXIT_FAILURE);
    }
    printf("Waiting for client on %s:%s\n", inet_ntoa(server_addr.sin_addr), argv[1]);

    // Accept client connection
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t client_addr_len = sizeof(client_addr);
    int client = accept(server, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client == -1)
    {
        perror("accept() failed");
        exit(EXIT_FAILURE);
    }
    printf("Connection from %s %d port [tcp/*] succeeded!\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // data receiving
    int c = 0;
    char buf[20], rec[21];
    // SOICTSOICT
    memset(buf, 'x', 9);
    // 0123456789
    memset(buf+9, 0, 11); 
   
    while (1)
    {
        memset(rec, 0, 21);
        int bytes_rec = recv(client, rec, 20, 0);
        rec[bytes_rec] = '\0';
        if(bytes_rec == -1){
            perror("recv() failed");
            return 1;
        }
        else if(bytes_rec == 0){
            printf("Count: %d\n", c);
            break;
        }

        strncat(buf, rec, 10);
        if(strstr(buf, "0123456789") != NULL){
            c++;
        }
        strcpy(buf, buf+10);
        strcat(buf, rec+10);
        if(strstr(buf, "0123456789") != NULL){
            c++;
        }
        strcpy(buf, buf+10);
        
    }

    close(client);
    close(server);
    return 0;
}