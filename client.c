#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9876
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE]; //buffer para guardar as mensagens

    // Create TCP socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid or unsupported address");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    // Main loop to send and receive messages
   while (1) {
    // Receive message from server
    ssize_t recv_len = recv(sockfd, buffer, BUFFER_SIZE, 0);
    if (recv_len <= 0) {
        perror("Error receiving message from server");
        break;
    }

    //termina as mensagens com um caractere null
    buffer[recv_len] = '\0';

    // Print received message
    printf("%s", buffer);

    // Check if client wants to end communication
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character

    if (strcmp(buffer, "0") == 0) {
        printf("You logged out. Communication ended.\n");
        send(sockfd, buffer, strlen(buffer), 0); // Send logout choice to server
        break;
    }

    // Send message to server
    send(sockfd, buffer, strlen(buffer), 0);
}


    // Close socket
    close(sockfd);

    return 0;
}
