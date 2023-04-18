#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000);

    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("connect() failed");
        return 1;
    }

    char mssv[10];
    char hoten[64];
    char ns[12];
    float dtb;
    char buf[256];
    
    
    while (1)
    {
        printf("Thong tin SV\n");
        printf("MSSV: ");
        scanf("%s", mssv);

        if(strncmp(mssv, "0000", 4) == 0){
            break;
        }

        getchar();

        printf("Ho Ten: ");
        fgets(hoten, sizeof(hoten), stdin);
        hoten[strlen(hoten) - 1] = 0;

        printf("NS: ");
        scanf("%s", ns);
        getchar();

        printf("DTB: ");
        scanf( "%f", &dtb);

        sprintf(buf, "%s %s %s %.2f", mssv, hoten, ns, dtb);

        send(client, buf, strlen(buf), 0);
    }
    
    close(client);

    return 0;
}