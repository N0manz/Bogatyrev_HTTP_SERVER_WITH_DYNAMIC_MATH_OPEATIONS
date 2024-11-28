#pragma once
#define SERVER_OPERATIONS_H


#include <netinet/in.h>
#include <unistd.h> // для закрытия сокетов
#define BUFFER_SIZE 1024
#define PORT 8080

typedef struct {
    char operation;   
    long num1;        
    long num2;        
} OperationData;

void handle_client(int client_socket);
int create_server_socket(struct sockaddr_in *server_addr);
void send_response(int client_socket, const char *message);
void run_server();
