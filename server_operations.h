#pragma once
#define SERVER_OPERATIONS_H


#include <netinet/in.h>
#include <unistd.h> // для закрытия сокетов
#define BUFFER_SIZE 1024
#define PORT 8080

void handle_client(int client_socket);
int create_server_socket(struct sockaddr_in *server_addr);
void send_response(int client_socket, const char *message);
void run_server();
