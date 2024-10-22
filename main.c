#include <stdio.h>
#include <stdlib.h>
#include "server_operations.h"


int main() {
    int server_socket, client_socket; // server - слушатель, client - разговорщик
    struct sockaddr_in server_addr, client_addr; // адресса клиента и сервера
    socklen_t client_len = sizeof(client_addr); // длина для вызова accept

    server_socket = create_server_socket(&server_addr); // создание сокета и привязка к нему адресса настройка для прослушки определенного порта

    printf("Server listening on port %d\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len); // создаем сокет клиента при его подключении благодаря функции accept
        if (client_socket < 0) {
            perror("Accept failed");
            close(server_socket);
            exit(EXIT_FAILURE);// благодаря stdlib.h можем юзать всё что связано с exit
        }

        handle_client(client_socket); // хендлер, с логикой калькулятора
    }

    close(server_socket);
    return 0;
}
