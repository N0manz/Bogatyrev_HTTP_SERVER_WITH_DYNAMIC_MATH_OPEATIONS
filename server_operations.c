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

    // Чтение данных от клиента
    read_size = recv(client_socket, buffer, BUFFER_SIZE, 0); 

    if (read_size > BUFFER_SIZE) {
        send_response(client_socket, "Error: Buffer overflow");
        fprintf(stderr, "Buffer overflow detected. Terminating connection.\n");
        exit(EXIT_FAILURE);  
    }

    if (read_size < 0) {
        perror("recv failed");
        send_response(client_socket, "Error: Failed to receive data");
        return;
    }

    buffer[read_size] = '\0';

    char *body = strstr(buffer, "\r\n\r\n");  
    if (body) {
        body += 4;  

        char operation;
        char num1_str[BUFFER_SIZE], num2_str[BUFFER_SIZE];
        long num1, num2;
        int error = 0;

        // Используем sscanf для извлечения символа операции и строк для чисел
        if (sscanf(body, "%c %s %s", &operation, num1_str, num2_str) == 3) {
            if (validate_number(num1_str, &num1) == -1) {
                send_response(client_socket, "Error: Invalid number format for the first operand");
                fprintf(stderr, "Invalid number format for the first operand: %s\n", num1_str);
                exit(EXIT_FAILURE);
            }

            if (validate_number(num2_str, &num2) == -1) {
                send_response(client_socket, "Error: Invalid number format for the second operand");
                fprintf(stderr, "Invalid number format for the second operand: %s\n", num2_str);
                exit(EXIT_FAILURE);
            }

            double result = process_operation(operation, num1, num2, &error);
            char response[BUFFER_SIZE];

            if (error) {
                snprintf(response, BUFFER_SIZE, "Error: Invalid operation or input");   
                send_response(client_socket, response);
                fprintf(stderr, "Invalid operation: %c between %ld and %ld\n", operation, num1, num2);
                exit(EXIT_FAILURE);
            }

            snprintf(response, BUFFER_SIZE, "%.2f", result);
            send_response(client_socket, response);
        } else {
            send_response(client_socket, "Error: Invalid request");
            fprintf(stderr, "Invalid request format: %s\n", body);
            exit(EXIT_FAILURE);
        }
    }

    close(client_socket);
}

int create_server_socket(struct sockaddr_in *server_addr) {
    int server_socket;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = INADDR_ANY;
    server_addr->sin_port = htons(PORT); // переводим значение порта в байты

    if (bind(server_socket, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) {
        perror("Binding failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 1) < 0) {
        perror("Listening failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    return server_socket;
}

void run_server() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_socket = create_server_socket(&server_addr);

    printf("Server listening on port %d\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Accept failed");
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        handle_client(client_socket);
    }

    close(server_socket);
}
