#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#define MAX_LENGTH 1024

int main()
{

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(9000);

    int client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == -1)
    {
        perror("socket() failed");
        return 1;
    }

    // if (connect(client, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    // {
    //     perror("connect() failed");
    //     return 1;
    // }

    // Nhan DL va tao BUFFER
    char name[64];
    int diskNo;
    char buf[1024];
    int pos = 0;

    
    printf("Enter Computer Name: ");
    fgets(name, sizeof(name), stdin);
    name[strlen(name) - 1] = 0;

    strcpy(buf, name);
    pos += strlen(name);
    buf[pos] = 0;
    pos++;

    printf("Enter Number of Disks: ");
    scanf("%d", &diskNo);
    getchar();

    char drive_letter;
    unsigned short drive_size;  

    for(int i=0; i<diskNo; i++){
        printf("Ky tu o Dia %d: ", i+1);
        scanf("%c /n", &drive_letter);
        buf[pos] = drive_letter;
        pos++;

        printf("Kich thuoc: ");
        scanf("%hd", &drive_size);
        getchar();
        memcpy(buf, &drive_size, sizeof(drive_size));
        pos += sizeof(drive_size);

    }

    printf("Bufffer size: %d /n", pos);

    send(client, buf, strlen(buf), 0);

    // Đóng kết nối socket
    close(client);

    return 0;
}