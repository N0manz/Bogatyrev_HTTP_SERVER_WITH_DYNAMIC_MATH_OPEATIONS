#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include "math_operations.h"
#include "server_operations.h"
#include "X.h"
#include <arpa/inet.h>
// ввыводим в поток ошибок соообщение о ошибке и закрываем сокет 
void log_and_close(int client_socket, const char *log_message, const char *response_message) {
    if (log_message) {
        fprintf(stderr, "%s\n", log_message); 
    }
    send_response(client_socket, response_message); 
    close(client_socket); 
}

// отправляем ответ пользователю, любое и плохое и хорошее
void send_response(int client_socket, const char *message) {
    send(client_socket, message, strlen(message), 0);
    close(client_socket);
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int read_size;

    // Читаем данные от клиента
    read_size = recv(client_socket, buffer, BUFFER_SIZE, 0);

    if (read_size > BUFFER_SIZE) {
        log_and_close(client_socket, "Buffer overflow detected. Terminating connection.", "Error: Buffer overflow");
        return;
    }

    if (read_size < 0) {
        log_and_close(client_socket, "recv failed", "Error: Failed to receive data");
        return;
    }

    buffer[read_size] = '\0';

    char *body = strstr(buffer, "\r\n\r\n"); // Переходим к телу запроса
    if (body) {
        body += 4; // Пропускаем \r\n\r\n

        char num1_str[BUFFER_SIZE], num2_str[BUFFER_SIZE];
        long num1, num2;
        char operation;
        int error = 0;

        // Извлекаем данные
        if (sscanf(body, "%c %s %s", &operation, num1_str, num2_str) == 3) {
            if (validate_number(num1_str, &num1) == -1) {
                log_and_close(client_socket, "Invalid number format for the first operand.", "Error: Invalid number format for the first operand");
                return;
            }

            if (validate_number(num2_str, &num2) == -1) {
                log_and_close(client_socket, "Invalid number format for the second operand.", "Error: Invalid number format for the second operand");
                return;
            }

            double result = process_operation(operation, num1, num2, &error);
            char response[BUFFER_SIZE];

            if (error) {
                log_and_close(client_socket, "Invalid operation or input.", "Error: Invalid operation or input");
                return;
            }

            snprintf(response, BUFFER_SIZE, "%.2f", result);
            send_response(client_socket, response);
        } else {
            log_and_close(client_socket, "Invalid request format.", "Error: Invalid request");
            return;
        }
    } else {
        log_and_close(client_socket, "Missing request body.", "Error: Missing request body");
    }
}



int create_server_socket(struct sockaddr_in *server_addr, int PORT) {
    int server_socket; 

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        fprintf(stderr, "Socket creation failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    server_addr->sin_family = AF_INET; // тип адресов, использую IPv4
    server_addr->sin_addr.s_addr = INADDR_ANY; // любой адрес
    server_addr->sin_port = htons(PORT); // переводим значение порта в байты и устанавливаем значение прослушиваемого порта 8080

    if (bind(server_socket, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) { // привязываем сервер к адресу
        fprintf(stderr, "Binding failed: %s\n", strerror(errno));
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 1) < 0) { // прослушиваем все входящие подключения
        fprintf(stderr, "Listening failed: %s\n", strerror(errno));
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    return server_socket; // всё ок и удалось создать серверный сокет
}
void run_server(int argc, char *argv[]) {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr); // чтобы для функции accept передать обёем необходимой памяти

    int PORT = 8080;
    if (argc == 3 && strstr(argv[1], "-p") != NULL) {
        char *endptr;
        errno = 0;
        long parsed_port = strtol(argv[2], &endptr, 10);

        if (errno != 0 || *endptr != '\0' || parsed_port <= 0) {
            fprintf(stderr, "Invalid port number\n");
            exit(EXIT_FAILURE);
        }
        PORT = (int)parsed_port;
    } else if (argc != 1) {
        exit(EXIT_FAILURE);
    }
    server_socket = create_server_socket(&server_addr, PORT); // создаем наш серверный сокет

    printf("Server listening on port %d\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len); // ждем пока какая-нибудь душа подключиться к серверу
        handle_client(client_socket); // логика работы с клиентом
    }

    close(server_socket); // закрываем сервер
}
