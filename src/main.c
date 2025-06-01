#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#include "chain.h"

#define PORT 8080
#define BUFFER_SIZE 1024
#define SHM_NAME "shm"
#define SHM_SIZE (1024 * 1024)

typedef struct {
    Chain chain;
    pthread_mutex_t mutex;
} SharedBlockchainData;

static SharedBlockchainData *data = NULL;

void dumpchain(int sock) {
    pthread_mutex_lock(&data->mutex);

    Chain* chain = &data->chain;

    char numblocks[20];
    snprintf(numblocks, sizeof(numblocks), "%d\n", chain->nblocks);
    send(sock, numblocks, strlen(numblocks), 0);

    for (int i = 0; i < chain->nblocks; i++) {
        Block *block = chain->blocks[i];

        if (block == NULL) {
            fprintf(
                stderr,
                "[CHILD %d] Warning: NULL block found at index %d in chain.\n",
                getpid(), i
            );
            continue;
        }

        char buf[0x2000];
        int len = serialize_block(block, buf, sizeof(buf));

        if (len <= 0) {
            fprintf(
                stderr,
                "[CHILD %d] Warning: Serialized length is %d for block %d\n",
                getpid(), len, i
            );
            continue;
        }

        char blocklen[20];
        snprintf(blocklen, sizeof(blocklen), "%d\n", len);
        send(sock, blocklen, strlen(blocklen), 0);

        send(sock, buf, (size_t) len, 0);
    }

    pthread_mutex_unlock(&data->mutex);
}

void docmd(int sock, char *cmd) {
    if (strcmp(cmd, "dump") == 0) {
        dumpchain(sock);
    }
}

void handle_client(int sock) {
    char buf[BUFFER_SIZE];
    ssize_t valread = recv(sock, buf, BUFFER_SIZE, 0);

    switch (valread) {
        case 0:
            printf("[CHILD %d] Client disconnected\n", getpid());
            break;
        case -1:
            perror("recv failed in child");
            break;
        default:
            printf(
                "[CHILD %d] Received %lu bytes from client\n",
                getpid(), valread
            );
            buf[valread] = '\0';
            docmd(sock, buf);
    }

    close(sock);
    printf("[CHILD %d] Client socket closed\n", getpid());
    exit(0);
}

void receive_loop(int fd) {
    int sock;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    pid_t pid;

    printf("[SERVER] Listening for incoming connections...\n");

    while(1) {
        if ((sock = accept(fd, (struct sockaddr *)&addr, &addrlen)) == -1) {
            perror("accept failed");
            continue;
        }
        printf("[SERVER] Accepted new connection (Client Socket FD: %d).\n", sock);

        pid = fork();
        if (pid == -1) {
            perror("fork failed");
            close(sock);
            continue;
        } else if (pid == 0) {
            close(fd);
            handle_client(sock);
        } else {
            close(sock);
            printf("[SERVER] Forked child process with PID: %d to handle client.\n", pid);
            while (waitpid(-1, NULL, WNOHANG) > 0);
        }
    }
}

int start_server() {
    int server;
    struct sockaddr_in addr;
    int opt = 1;

    if ((server = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        return -1;
    }

    printf("[SERVER] Socket created (FD: %d)\n", server);

    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server);
        return -1;
    }

    printf("[SERVER] Socket options set.\n");

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        perror("bind failed");
        close(server);
        return -1;
    }

    printf("[SERVER] Socket bound to port %d.\n", PORT);

    if (listen(server, 10) == -1) {
        perror("listen failed");
        close(server);
        return -1;
    }

    return server;
}

int main() {
    int shm;

    shm = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm == -1) {
        perror("shm_open failed");
        return EXIT_FAILURE;
    }

    printf(
        "[*] Shared memory object created/opened: %s (FD: %d)\n",
        SHM_NAME,
        shm
    );

    if (ftruncate(shm, SHM_SIZE) == -1) {
        perror("ftruncate failed");
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }

    printf(
        "[*] Shared memory size set to %lu bytes.\n",
        (unsigned long) SHM_SIZE
    );

    data = (SharedBlockchainData*) mmap(
        NULL,
        SHM_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        shm,
        0
    );

    if (data == MAP_FAILED) {
        perror("mmap failed");
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }

    printf("[*] Shared memory mapped at addr: %p\n", (void*) data);

    close(shm);

    printf("[*] Initializing chain\n");
    data->chain = *create_chain();

    int server = start_server();
    if (server == -1) {
        fprintf(stderr, "[SERVER] Failed to start server\n");

        if (data != MAP_FAILED) {
            munmap(data, SHM_SIZE);
        }
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }

    receive_loop(server);

    printf("[SERVER] Server shutting down\n");

    close(server);

    if (data != MAP_FAILED) {
        printf("[*] Unmapping shared memory\n");
        munmap(data, SHM_SIZE);
    }

    printf("[*] Unlinking shared memory object: %s\n", SHM_NAME);

    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink failed");
    }

    while (waitpid(-1, NULL, WNOHANG) > 0);

    return 0;
}