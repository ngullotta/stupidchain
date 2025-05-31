
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_LINE_LEN 256

#define GET_BLOCKCHAIN_CMD "D"

ssize_t read_line(int sock, char* buffer, size_t buffer_size) {
    size_t i = 0;
    ssize_t bytes_read;
    char c;

    while (i < buffer_size - 1) {
        bytes_read = read(sock, &c, 1);
        if (bytes_read <= 0) {
            return bytes_read;
        }
        buffer[i++] = c;
        if (c == '\n') {
            break;
        }
    }
    buffer[i] = '\0';
    return i;
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char line_buffer[MAX_LINE_LEN];

    printf("Enter command:\n");
    while(1) {
        printf("> ");
        char client_command[50];
        if (fgets(client_command, sizeof(client_command), stdin) == NULL) {
            fprintf(stderr, "Error reading command.\n");
            exit(EXIT_FAILURE);
        }
        client_command[strcspn(client_command, "\n")] = 0;

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Socket creation error");
            exit(EXIT_FAILURE);
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
            perror("Invalid address/ Address not supported");
            close(sock);
            exit(EXIT_FAILURE);
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("Connection Failed");
            close(sock);
            exit(EXIT_FAILURE);
        }

        printf("Connected to blockchain server at %s:%d\n", SERVER_IP, PORT);

        if (strcmp(client_command, GET_BLOCKCHAIN_CMD) == 0) {

            printf("[CLIENT] Requesting blockchain dump...\n");
            send(sock, GET_BLOCKCHAIN_CMD, strlen(GET_BLOCKCHAIN_CMD), 0);

            ssize_t bytes_read = read_line(sock, line_buffer, sizeof(line_buffer));
            if (bytes_read <= 0) {
                fprintf(stderr, "Error or EOF receiving number of blocks.\n");
                close(sock);
                exit(EXIT_FAILURE);
            }
            int num_blocks = atoi(line_buffer);
            printf("[CLIENT] Server reports %d blocks.\n", num_blocks);

            for (int i = 0; i < num_blocks; i++) {

                bytes_read = read_line(sock, line_buffer, sizeof(line_buffer));
                if (bytes_read <= 0) {
                    fprintf(stderr, "Error or EOF receiving block %d length.\n", i);
                    break;
                }
                int block_len = atoi(line_buffer);
                printf("[CLIENT] Receiving block %d (length %d bytes)...\n", i, block_len);

                if (block_len > 0) {
                    if (block_len >= BUFFER_SIZE) {
                        fprintf(stderr, "Error: Received block length (%d) exceeds client buffer size (%d).\n", block_len, BUFFER_SIZE);
                        close(sock);
                        exit(EXIT_FAILURE);
                    }
                    ssize_t total_received = 0;
                    while (total_received < block_len) {
                        bytes_read = read(sock, buffer + total_received, block_len - total_received);
                        if (bytes_read <= 0) {
                            fprintf(stderr, "Error or EOF while reading block %d data.\n", i);
                            break;
                        }
                        total_received += bytes_read;
                    }
                    buffer[total_received] = '\0';
                    printf("[CLIENT] --- Received Block %d ---\n%s\n----------------------\n", i, buffer);
                } else {
                    printf("[CLIENT] Block %d is empty or failed to serialize on server.\n", i);
                }
            }

            bytes_read = read(sock, buffer, BUFFER_SIZE - 1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                printf("[CLIENT] Server final response: %s\n", buffer);
            }

        } else {
            send(sock, client_command, strlen(client_command), 0);
            close(sock);
            break;
        }

        close(sock);
    }

    return 0;
}
