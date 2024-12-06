#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    int socket;
    char name[50];
} Client;

Client clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast_message(char* message, int sender_socket) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (clients[i].socket != sender_socket) {
            send(clients[i].socket, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void* handle_client(void* arg) {
    Client* client = (Client*)arg;
    char buffer[BUFFER_SIZE];
    int read_size;

    while ((read_size = recv(client->socket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[read_size] = '\0';
        char message[BUFFER_SIZE + 50];
        snprintf(message, sizeof(message), "<%s> %s", client->name, buffer);
        printf("%s", message);
        broadcast_message(message, client->socket);
    }

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (clients[i].socket == client->socket) {
            while (i < client_count - 1) {
                clients[i] = clients[i + 1];
                i++;
            }
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    close(client->socket);
    free(client);
    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    pthread_t thread_id;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    listen(server_socket, 5);
    printf("Server listening on port %d...\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        if (client_count >= MAX_CLIENTS) {
            printf("Maximum clients reached. Connection rejected.\n");
            close(client_socket);
            continue;
        }

        Client* new_client = malloc(sizeof(Client));
        new_client->socket = client_socket;
        recv(client_socket, new_client->name, sizeof(new_client->name), 0);

        pthread_mutex_lock(&clients_mutex);
        clients[client_count++] = *new_client;
        pthread_mutex_unlock(&clients_mutex);

        pthread_create(&thread_id, NULL, handle_client, (void*)new_client);
    }

    close(server_socket);
    return 0;
}