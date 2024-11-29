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

void send_response(int client_socket, const char *message) {
    send(client_socket, message, strlen(message), 0);
    close(client_socket);
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int read_size;

    // читаем данные от клиента 
    read_size = recv(client_socket, buffer, BUFFER_SIZE, 0);

    if (read_size > BUFFER_SIZE) {
        send_response(client_socket, "Error: Buffer overflow");
        fprintf(stderr, "Buffer overflow detected. Terminating connection.\n");
        close(client_socket); // Закрываем клиентский сокет перед завершением
        return;  
    }

    if (read_size < 0) {
        perror("recv failed");
        send_response(client_socket, "Error: Failed to receive data");
        close(client_socket); // Закрываем клиентский сокет в случае ошибки, если не удалось считать
        return;
    }

    buffer[read_size] = '\0';

    char *body = strstr(buffer, "\r\n\r\n"); // вообще как понял скипаем http заголовок и переходим к телу и возвращаем указатель на начало этого \r\n\r\n
    if (body) {
        body += 4; // перемещаем наш указатель на 4 вперед скипая это чудо

        OperationData op_data; // Используем структуру OperationData
        char num1_str[BUFFER_SIZE], num2_str[BUFFER_SIZE];
        int error = 0;

        // Используем sscanf для извлечения символа операции и строк для чисел
        if (sscanf(body, "%c %s %s", &op_data.operation, num1_str, num2_str) == 3) {
            // Валидация первого числа
            if (validate_number(num1_str, &op_data.num1) == -1) {
                send_response(client_socket, "Error: Invalid number format for the first operand"); // то что отправляем клиенту
                fprintf(stderr, "Invalid number format for the first operand: %s\n", num1_str); // то что выводим в своей консоли, fprintf чтобы выводить в поток с ошибками
                close(client_socket); // Закрываем клиентский сокет
                return;
            }

            // Валидация второго числа
            if (validate_number(num2_str, &op_data.num2) == -1) {
                send_response(client_socket, "Error: Invalid number format for the second operand");
                fprintf(stderr, "Invalid number format for the second operand: %s\n", num2_str);
                close(client_socket); // Закрываем клиентский сокет
                return;
            }

            // Выполняем операцию
            double result = process_operation(op_data.operation, op_data.num1, op_data.num2, &error);
            char response[BUFFER_SIZE];

            if (error == 1) {   
                send_response(client_socket, "Error: Invalid result");
                fprintf(stderr, "Invalid result for operation: %c %ld %ld\n", op_data.operation, op_data.num1, op_data.num2);
                close(client_socket); // Закрываем клиентский сокет
                return;
            } 

            // Формируем и отправляем ответ
            snprintf(response, BUFFER_SIZE, "%.2f", result);
            send_response(client_socket, response);
        } else {
            send_response(client_socket, "Error: Invalid request");
            fprintf(stderr, "Invalid request format: %s\n", body);
            close(client_socket); // Закрываем клиентский сокет
            return;
        }
    } else {
        send_response(client_socket, "Error: Missing request body");
        fprintf(stderr, "Missing request body in: %s\n", buffer);
        close(client_socket); // Закрываем клиентский сокет
    }

    close(client_socket);
}

int create_server_socket(struct sockaddr_in *server_addr) {
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

void run_server() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr); // чтобы для функции accept передать обёем необходимой памяти

    server_socket = create_server_socket(&server_addr); // создаем наш серверный сокет

    printf("Server listening on port %d\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len); // ждем пока какая-нибудь душа подключиться к серверу
        if (client_socket < 0) {
            fprintf(stderr, "Accept failed: %s\n", strerror(errno));
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        handle_client(client_socket); // логика работы с клиентом
    }

    close(server_socket); // закрываем сервер
}
