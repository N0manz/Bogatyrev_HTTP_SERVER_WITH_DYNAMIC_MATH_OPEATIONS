#pragma once

#include <netinet/in.h>// всё для работы с сокетами(структура sockaddr_in)
#include <unistd.h> // для закрытия сокетов

// Определения
#define PORT 8080
#define BUFFER_SIZE 1024  // Размер буфера

// Прототипы функций
void handle_client(int client_socket);
int create_server_socket(struct sockaddr_in *server_addr);

