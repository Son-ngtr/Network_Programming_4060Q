#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <poll.h>

#define BUFFER_SIZE 1024
#define TIMEOUT 10

void receive_message(int sockfd, struct sockaddr_in* sender_addr) {
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(struct sockaddr_in);
    ssize_t recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                                (struct sockaddr*)sender_addr, &addr_len);
    if (recv_len > 0) {
        buffer[recv_len] = '\0';
        printf("Received message: %s\n", buffer);
    }
}

void send_message(int sockfd, const char* receiver_ip, int receiver_port, const char* message) {
    struct sockaddr_in receiver_addr;
    memset(&receiver_addr, 0, sizeof(struct sockaddr_in));
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(receiver_port);
    inet_pton(AF_INET, receiver_ip, &(receiver_addr.sin_addr));
    sendto(sockfd, message, strlen(message), 0,
           (struct sockaddr*)&receiver_addr, sizeof(struct sockaddr_in));
    printf("Sent message: %s\n", message);
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Usage: %s <receiver_ip> <receiver_port> <listening_port>\n", argv[0]);
        return 1;
    }

    const char* receiver_ip = argv[1];
    int receiver_port = atoi(argv[2]);
    int listening_port = atoi(argv[3]);

    // Khởi tạo socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        return 1;
    }

    // Cấu hình địa chỉ máy nghe
    struct sockaddr_in listening_addr;
    memset(&listening_addr, 0, sizeof(struct sockaddr_in));
    listening_addr.sin_family = AF_INET;
    listening_addr.sin_addr.s_addr = INADDR_ANY;
    listening_addr.sin_port = htons(listening_port);

    // Gắn socket với địa chỉ máy nghe
    if (bind(sockfd, (struct sockaddr*)&listening_addr, sizeof(struct sockaddr_in)) < 0) {
        perror("Error binding socket");
       
        close(sockfd);
        return 1;
    }

    // Mảng lưu trữ các socket được theo dõi
    struct pollfd fds[2];
    fds[0].fd = sockfd;
    fds[0].events = POLLIN; // Đợi sự kiện đọc dữ liệu
    fds[1].fd = 0;
    fds[1].events = POLLIN; // Đợi sự kiện đọc từ bàn phím

    printf("New chat is generating ...\n");

    while (1) {
        // Sử dụng hàm poll để theo dõi các sự kiện trên các socket
        int ready = poll(fds, 2, TIMEOUT * 1000);

        if (ready == -1) {
            perror("Error in poll");
            break;
        // } else if (ready == 0) {
        //     printf("Timeout. No activity.\n");
        } else {
            // Kiểm tra sự kiện từ socket nhận tin nhắn
            if (fds[0].revents & POLLIN) {
                struct sockaddr_in sender_addr;
                receive_message(sockfd, &sender_addr);
            }

            // Kiểm tra sự kiện từ bàn phím
            if (fds[1].revents & POLLIN) {
                char input[BUFFER_SIZE];
                fgets(input, BUFFER_SIZE, stdin);
                // Xóa ký tự newline từ input
                input[strcspn(input, "\n")] = 0;
                send_message(sockfd, receiver_ip, receiver_port, input);
            }
        }
    }

    close(sockfd);

    return 0;
}
