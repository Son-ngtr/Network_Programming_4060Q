#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

void handle_client(int client_socket, int *connected_clients);
void normalize_string(char *str);

int main() {
    int server_socket, client_socket;
    int connected_clients = 0;
    struct sockaddr_in server_addr, client_addr;
    fd_set read_fds, all_fds;
    int max_fd, activity, i;
    int opt = 1;
    int addrlen = sizeof(server_addr);

    // Create server socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse address and port
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Failed to set socket options");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8888);

    // Bind the socket to localhost:8888
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 3) < 0) {
        perror("Failed to listen for connections");
        exit(EXIT_FAILURE);
    }

    // Initialize file descriptor sets
    FD_ZERO(&all_fds);
    FD_SET(server_socket, &all_fds);
    max_fd = server_socket;

    printf("Server started. Waiting for connections...\n");

    while (1) {
        // Select the active file descriptors
        read_fds = all_fds;
        activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("Failed to select file descriptors");
            exit(EXIT_FAILURE);
        }

        // Check for new connection
        if (FD_ISSET(server_socket, &read_fds)) {
            if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen)) < 0) {
                perror("Failed to accept connection");
                exit(EXIT_FAILURE);
            }

            // Send greeting message to client
            char greeting_msg[BUFFER_SIZE];
            sprintf(greeting_msg, "Xin chào. Hiện có %d clients đang kết nối.\n", connected_clients);
            send(client_socket, greeting_msg, strlen(greeting_msg), 0);

            // Add new client socket to set
            FD_SET(client_socket, &all_fds);

            if (client_socket > max_fd)
                max_fd = client_socket;

            connected_clients++;

            printf("New client connected. Socket fd: %d, IP: %s, Port: %d\n",
                   client_socket,
                   inet_ntoa(client_addr.sin_addr),
                   ntohs(client_addr.sin_port));
        }

        // Check for IO activity on client sockets
        for (i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &read_fds)) {
                // Handle client request
                if (i == server_socket)
                    continue;

                handle_client(i, &connected_clients);
            }
        }
    }

    return 0;
}

void handle_client(int client_socket, int *connected_clients) {
    char buffer[BUFFER_SIZE];
    int read_size;

    // Receive client message
    if ((read_size = recv(client_socket, buffer, BUFFER_SIZE, 0)) == 0) {
        // Client disconnected
        getpeername(client_socket, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen);
        printf("Client disconnected. IP: %s, Port: %d\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));

        // Close client socket and remove from set
        close(client_socket);
        FD_CLR(client_socket, &all_fds);

        (*connected_clients)--;
        return;
    }

    // Normalize client message
    normalize_string(buffer);

    // Send normalized message back to client
    send(client_socket, buffer, strlen(buffer), 0);
}

void normalize_string(char *str) {
    int i, j;
    int space_flag = 0;
    int str_length = strlen(str);

    // Remove leading and trailing spaces
    while (isspace(str[0]))
        memmove(str, str + 1, strlen(str));

    while (isspace(str[str_length - 1]))
        str[--str_length] = '\0';

    // Normalize string
    for (i = 0, j = 0; i < str_length; i++) {
        if (isspace(str[i])) {
            if (!space_flag) {
                str[j++] = ' ';
                space_flag = 1;
            }
        } else {
            str[j++] = tolower(str[i]);
            space_flag = 0;
        }
    }

    str[j] = '\0';

    // Capitalize first letter of each word
    for (i = 0; i < j; i++) {
        if (i == 0 || isspace(str[i - 1]))
            str[i] = toupper(str[i]);
    }
}
