#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void* receive_messages(void* socket_ptr) {
    int socket = *((int*)socket_ptr);
    char buffer[BUFFER_SIZE];
    int read_size;

    while ((read_size = recv(socket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[read_size] = '\0';
        printf("%s", buffer);
    }

    return NULL;
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char name[50], message[BUFFER_SIZE];
    pthread_t receive_thread;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;
    send(client_socket, name, strlen(name), 0);

    pthread_create(&receive_thread, NULL, receive_messages, &client_socket);

    while (1) {
        fgets(message, BUFFER_SIZE, stdin);
        if (strcmp(message, "exit\n") == 0) {
            break;
        }
        send(client_socket, message, strlen(message), 0);
    }

    close(client_socket);
    return 0;
}