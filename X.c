#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>  // Для INT_MAX и INT_MIN
#include <errno.h>   // Для проверки ошибок strtol
#include "math_operations.h"
#include "X.h"


/*Правки от Павла
После любой ошибки сервер закрывается
Добавление дополнительных операций (l, ^ разрешил использовать math.h)
использую  один символ, так как char
*/


void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int read_size;
 
    // Чтение данных от клиента
    read_size = recv(client_socket, buffer, BUFFER_SIZE, 0);
    
    // Проверка на переполнение буфера
    if (read_size > BUFFER_SIZE) {
        printf("Buffer overflow attempt detected.\n");
        char response[] = "Error: Buffer overflow";
        send(client_socket, response, strlen(response), 0);
        close(client_socket);
        exit(EXIT_FAILURE);  // Закрываем сервер при возникновении ошибки( об этом меня попросил Павел))
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
        char *endptr; // символ окончания преобразования

        // Используем sscanf для извлечения символа операции и строк для чисел
        if (sscanf(body, "%c %s %s", &operation, num1_str, num2_str) == 3) {
            // Преобразуем строки в long и проверяем на переполнение
            errno = 0;  // Сбрасываем errno перед вызовом strtol
            num1 = strtol(num1_str, &endptr, 10);
            if ((errno == ERANGE) || (num1 > INT_MAX || num1 < INT_MIN) || *endptr != '\0') {
                char response[BUFFER_SIZE];
                snprintf(response, BUFFER_SIZE, "Error: Number 1 out of range");
                send(client_socket, response, strlen(response), 0);
                close(client_socket);
                exit(EXIT_FAILURE);  // Закрываем сервер при возникновении ошибки
            }

            errno = 0;  // Сбрасываем errno перед вызовом strtol
            num2 = strtol(num2_str, &endptr, 10);
            if ((errno == ERANGE && (num2 == LONG_MAX || num2 == LONG_MIN)) || (num2 > INT_MAX || num2 < INT_MIN) || *endptr != '\0') {
                char response[BUFFER_SIZE];
                snprintf(response, BUFFER_SIZE, "Error: Number 2 out of range");
                send(client_socket, response, strlen(response), 0);
                close(client_socket);
                exit(EXIT_FAILURE);  // Закрываем сервер при возникновении ошибки
            }

            double result = 0;
            char response[BUFFER_SIZE];
            int error = 0;

            switch (operation) {
                case '+':
                    result = add(num1, num2);
                    break;
                case '-':
                    result = subtract(num1, num2);
                    break;
                case '*':
                    result = multiply(num1, num2);
                    break;
                case '/':
                    result = divide(num1, num2, &error);
                    if (error) {
                        snprintf(response, BUFFER_SIZE, "Error: Division by 0");
                        send(client_socket, response, strlen(response), 0);
                        close(client_socket);
                        exit(EXIT_FAILURE);
                    }
                    break;
                case 'l': 
                    result = logarithm(num1, num2, &error);
                    if (error) {
                        snprintf(response, BUFFER_SIZE, "Error: Invalid input for log");
                        send(client_socket, response, strlen(response), 0);
                        close(client_socket);
                        exit(EXIT_FAILURE);
                    }
                    break;
                case '^':  
                    result = power(num1, num2);
                    break;
                default:
                    snprintf(response, BUFFER_SIZE, "Error: Invalid operation");
                    send(client_socket, response, strlen(response), 0);
                    close(client_socket);
                    exit(EXIT_FAILURE);
            }

            snprintf(response, BUFFER_SIZE, "%.2f", result);
            send(client_socket, response, strlen(response), 0);
        } else {
            char response[BUFFER_SIZE];
            snprintf(response, BUFFER_SIZE, "Error: Invalid request");
            send(client_socket, response, strlen(response), 0);
            close(client_socket);
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
