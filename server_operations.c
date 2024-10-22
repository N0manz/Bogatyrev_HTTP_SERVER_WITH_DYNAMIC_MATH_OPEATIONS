#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include "math_operations.h"
#include "server_operations.h"
#include "X.h"

void send_response(int client_socket, const char *message) {
    send(client_socket, message, strlen(message), 0);
    close(client_socket);
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int read_size;

    // Чтение данных от клиента
    read_size = recv(client_socket, buffer, BUFFER_SIZE, 0);
    
    // Проверка на переполнение буфера
    if (read_size > BUFFER_SIZE) {
        send_response(client_socket, "Error: Buffer overflow");
        exit(EXIT_FAILURE);  // Закрываем сервер при возникновении ошибки
    }

    if (read_size < 0) {
        return;
    }

    buffer[read_size] = '\0';

    char *body = strstr(buffer, "\r\n\r\n");
    if (body) {
        body += 4;  // Пропускаем символы \r\n\r\n

        char operation;
        char num1_str[BUFFER_SIZE], num2_str[BUFFER_SIZE];
        long num1, num2;
        int error = 0;

        // Используем sscanf для извлечения символа операции и строк для чисел
        if (sscanf(body, "%c %s %s", &operation, num1_str, num2_str) == 3) {
            // Проверяем первое число
            if (validate_number(num1_str, &num1) == -1) {
                exit(EXIT_FAILURE);
            }

            // Проверяем второе число
            if (validate_number(num2_str, &num2) == -1) {
                exit(EXIT_FAILURE);
            }

            double result = process_operation(operation, num1, num2, &error);
            char response[BUFFER_SIZE];

            if (error) {
                snprintf(response, BUFFER_SIZE, "Error: Invalid operation or input");
                send_response(client_socket, response);
                exit(EXIT_FAILURE);
            }

            snprintf(response, BUFFER_SIZE, "%.2f", result);
            send_response(client_socket, response);
        } else {
            send_response(client_socket, "Error: Invalid request");
            exit(EXIT_FAILURE);
        }
    }

    close(client_socket);
}

int create_server_socket(struct sockaddr_in *server_addr) {
    int server_socket;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        exit(EXIT_FAILURE);
    }

    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = INADDR_ANY;
    server_addr->sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) {
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 1) < 0) {
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    return server_socket;
}
