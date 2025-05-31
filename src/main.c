#include <stdio.h>      // For printf, perror
#include <stdlib.h>     // For exit
#include <string.h>     // For memset, strlen
#include <unistd.h>     // For close
#include <arpa/inet.h>  // For inet_pton
#include <sys/socket.h> // For socket, bind, listen, accept, send, recv
#include "chain.c"

#define PORT 8080        // The port on which the server will listen
#define BUFFER_SIZE 1024 // Size of the receive buffer

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    const char *hello_message = "Hello from server! What's up?";

    // 1. Create socket file descriptor
    // AF_INET: IPv4 Internet protocols
    // SOCK_STREAM: Provides sequenced, reliable, two-way, connection-based byte streams (TCP)
    // 0: Default protocol for SOCK_STREAM (TCP)
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    printf("Server: Socket created (FD: %d)\n", server_fd);

    // Optional: Set socket options to reuse address and port
    // This helps avoid "Address already in use" errors if you restart the server quickly
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server_fd); // Clean up
        exit(EXIT_FAILURE);
    }
    printf("Server: Socket options set.\n");

    // Prepare the sockaddr_in structure for binding
    memset(&address, 0, sizeof(address)); // Clear structure
    address.sin_family = AF_INET;         // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Listen on all available network interfaces
                                          // (0.0.0.0, allowing connections from any IP)
    address.sin_port = htons(PORT);       // Convert port to network byte order

    // 2. Bind the socket to the specified IP address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server: Socket bound to port %d.\n", PORT);

    // 3. Listen for incoming connections
    // 10: The maximum number of pending connections queue will hold
    if (listen(server_fd, 10) == -1) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server: Listening for incoming connections...\n");

    // 4. Accept an incoming connection
    // This call blocks until a client attempts to connect.
    // It returns a new socket file descriptor (new_socket) for communication with this client.
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == -1) {
        perror("accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server: Accepted new connection (Client Socket FD: %d).\n", new_socket);

    // 5. Send data to the client
    send(new_socket, hello_message, strlen(hello_message), 0);
    printf("Server: Sent message to client.\n");

    // 6. Receive data from the client
    ssize_t valread = recv(new_socket, buffer, BUFFER_SIZE, 0);
    if (valread == -1) {
        perror("recv failed");
    } else if (valread == 0) {
        printf("Server: Client disconnected.\n");
    } else {
        buffer[valread] = '\0'; // Null-terminate the received data
        printf("Server: Received from client: %s\n", buffer);
    }

    // 7. Close sockets
    close(new_socket); // Close the client-specific communication socket
    printf("Server: Client socket closed.\n");
    close(server_fd);  // Close the listening socket
    printf("Server: Listening socket closed. Server shutting down.\n");

    return 0;
}