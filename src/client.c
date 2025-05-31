#include <stdio.h>      // For printf, perror
#include <stdlib.h>     // For exit
#include <string.h>     // For memset, strlen
#include <unistd.h>     // For close
#include <arpa/inet.h>  // For inet_pton
#include <sys/socket.h> // For socket, connect, send, recv

#define SERVER_IP "127.0.0.1" // The IP address of the server (localhost)
#define PORT 8080             // The port the server is listening on
#define BUFFER_SIZE 1024      // Size of the receive buffer

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    const char *client_message = "Hello from client!";

    // 1. Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket creation error");
        exit(EXIT_FAILURE);
    }
    printf("Client: Socket created (FD: %d)\n", sock);

    // Prepare the sockaddr_in structure for connecting
    memset(&serv_addr, 0, sizeof(serv_addr)); // Clear structure
    serv_addr.sin_family = AF_INET;           // IPv4
    serv_addr.sin_port = htons(PORT);         // Convert port to network byte order

    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("Client: Server address prepared (%s:%d).\n", SERVER_IP, PORT);

    // 2. Connect to the server
    // This call attempts to establish a connection to the specified server.
    // It will block until a connection is established or an error occurs.
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("Client: Connected to server %s:%d.\n", SERVER_IP, PORT);

    // 3. Send data to the server
    send(sock, client_message, strlen(client_message), 0);
    printf("Client: Sent message to server.\n");

    // 4. Receive data from the server
    ssize_t valread = recv(sock, buffer, BUFFER_SIZE, 0);
    if (valread == -1) {
        perror("recv failed");
    } else if (valread == 0) {
        printf("Client: Server disconnected.\n");
    } else {
        buffer[valread] = '\0'; // Null-terminate the received data
        printf("Client: Received from server: %s\n", buffer);
    }

    // 5. Close socket
    close(sock);
    printf("Client: Socket closed. Client shutting down.\n");

    return 0;
}