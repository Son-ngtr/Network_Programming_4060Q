#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

int main()
{
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind() failed");
        return 1;        
    }

    if (listen(listener, 5))
    {
        perror("listen() failed");
        return 1;
    }

    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);

    int client = accept(listener, (struct sockaddr *)&client_addr, &client_addr_len);

    printf("Accepted socket %d from IP: %s:%d\n", client, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    char mssv[10];
    char hoten[64];
    char ns[12];
    float dtb;
    char buf[256];

    FILE *f = fopen("sv_log.txt", "a");

    while (1)
    {
        int ret = recv(client, buf, sizeof(buf), 0);
        if(ret <= 0){
            break;
        }

        buf[ret] = 0;
        
        // Tach Xau
        memcpy(mssv, buf, 8);
        mssv[8] = 0;

        int hoten_len = ret - 25;
        memcpy(hoten, buf + 9, hoten_len);
        hoten[hoten_len] = 0;

        memcpy(ns, buf + 10 + hoten_len, 10);
        ns[10] = 0;

        dtb = atof(buf + hoten_len + 20);

        sprintf(buf, "%s %s %s %.2f \n", mssv, hoten, ns, dtb);

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);



        fprintf(f, "%s %d-%d-%d %d:%d:%d %s", 
            inet_ntoa(client_addr.sin_addr),
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, buf);

    }
    

    

    close(client);
    close(listener);
}