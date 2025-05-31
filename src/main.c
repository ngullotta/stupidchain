#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "chain.h"

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    printf("[*] Initializing chain...\n");
    Chain* chain = create_chain();
    printf("[*] Chain initialized\n");

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    printf("[SERVER] Socket created (FD: %d)\n", server_fd);

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("[SERVER] Socket options set.\n");

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;

    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("[SERVER] Socket bound to port %d.\n", PORT);

    if (listen(server_fd, 10) == -1) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("[SERVER] Listening for incoming connections...\n");

    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == -1) {
            perror("accept failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
        printf("[SERVER] Accepted new connection (Client Socket FD: %d).\n", new_socket);


        ssize_t valread = recv(new_socket, buffer, BUFFER_SIZE, 0);
        if (valread == -1) {
            perror("recv failed");
        } else if (valread == 0) {
            printf("[SERVER] Client disconnected.\n");
        } else {
            buffer[valread] = '\0';
            printf("[SERVER] Received from client: %s\n", buffer);

            if (strcmp(buffer, "exit") == 0) {
                close(new_socket);
                printf("[SERVER] Client socket closed.\n");
                break;
            }


            if (strcmp(buffer, "D") == 0) {
                char num_blocks_str[20];
                snprintf(num_blocks_str, sizeof(num_blocks_str), "%d\n", chain->nblocks);
                send(new_socket, num_blocks_str, strlen(num_blocks_str), 0);
                printf("[SERVER] Sent number of blocks: %d\n", chain->nblocks);
                for (int i = 0; i < chain->nblocks; i++) {
                    Block *block = chain->blocks[i];
                    if (block == NULL) {
                        fprintf(stderr, "Warning: NULL block found at index %d in chain.\n", i);
                        continue;
                    }

                    char msgbuf[9128];
                    int len = serialize_block(block, msgbuf, sizeof(msgbuf));

                    if (len == 0) {
                        fprintf(stderr, "Warning: Serialized length is 0 for block %d\n", i);
                        continue;
                    }

                    char block_len_str[20];
                    snprintf(block_len_str, sizeof(block_len_str), "%d\n", len);
                    send(new_socket, block_len_str, strlen(block_len_str), 0);
                    printf("[SERVER] Sent block %d length: %d\n", block->index, len);

                    send(new_socket, msgbuf, strlen(msgbuf), 0);
                    printf("[SERVER] Sent message to client.\n");
                }
                const char *response_msg = "Blockchain dump complete.";
                send(new_socket, response_msg, strlen(response_msg), 0);
            }
        }
    }
    close(server_fd);
    printf("[SERVER] Listening socket closed. Server shutting down.\n");

    free_chain(chain);

    return 0;
}