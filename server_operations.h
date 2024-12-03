#pragma once



#include <netinet/in.h>
#include <unistd.h> // для закрытия сокетов
#define BUFFER_SIZE 1024
typedef struct {
    char operation;   
    long num1;        
    long num2;        
} OperationData;
void log_and_close(int client_socket, const char *log_message, const char *response_message);
void handle_client(int client_socket);
int create_server_socket(struct sockaddr_in *server_addr, int PORT);
void send_response(int client_socket, const char *message);
void run_server(int argc, char *argv[]);
