#include <unistd.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <arpa/inet.h>
#include <stdio.h> 
#include "serv_functions.c"

// TCP-порт сервера
#define PORT 1200


int main(void)
{
    int server_fd;  //дескриптор прослушивающего сокета
    int newsock_fd; //дескриптор присоединенного сокета

    int bTerminate = 0;

    // Создаем сокет
    // Для TCP-сокета указываем параметр SOCK_STREAM
    // Для UDP - SOCK_DGRAM
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        fprintf(stderr, "Can't create socket\n");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    // Заполняем структуру адресов
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = 0;
    serv_addr.sin_port = htons((u_short) PORT);
    // Связываем сокет с заданным сетевым интерфесом и портом
    if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        fprintf(stderr, "Can't bind\n");
        exit(1);
    }

    // Переводим сокет в режим прослушивания заданного порта
    // с максимальным количеством ожидания запросов на соединение 100
    if (listen(server_fd, 100) < 0)
    {
        fprintf(stderr, "Can't listen\n");
        exit(1);
    }
    printf("Server is listening on %s:%d\n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));

    // Основной цикл обработки подключения клиентов
    while (!bTerminate)
    {
        printf("Wait for connections.....\n");
        struct sockaddr_in client_addr;
        int addrlen = sizeof(client_addr);
        memset(&client_addr, 0, addrlen);
        // Переводим сервис в режим ожидания запроса на соединение.
        // Вызов синхронный, т.е. возвращает управление только при
        // подключении клиента или ошибке
        if ((newsock_fd = 
            accept(server_fd, (struct sockaddr *) &client_addr, &addrlen)) < 0)
        {
            fprintf(stderr, "Can't accept connection\n");
            break;
        }
        // Получаем параметры присоединенного сокета NS и
        // информацию о клиенте
        addrlen = sizeof(serv_addr);
        getsockname(newsock_fd, (struct sockaddr *) &serv_addr, &addrlen);
        // Функция inet_ntoa возвращает указатель на глобальный буффер,
        // поэтому использовать ее в одном вызове printf не получится
        printf("Accepted connection on %s:%d ",
               inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
        printf("from client %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        send_string(newsock_fd, "******* Welcome to the most anonymous server in the world! *******\r\n=> We don't store your data 'cause we don't have enough storage <=\r\n\n\n");

        char sReceiveBuffer[1024] = {0};
        // Получаем и обрабатываем данные от клиента
        while (1)
        {
            int nReaded = recv(newsock_fd, sReceiveBuffer, sizeof(sReceiveBuffer) - 1, 0);
            // В случае ошибки (например, отсоединения клиента) выходим
            if (nReaded <= 0)
                break;
            // Мы получаем поток байт, поэтому нужно самостоятельно
            // добавить завершающий 0 для ASCII строки
            sReceiveBuffer[nReaded] = 0;
            // Отбрасываем символы превода строк
            for (char *pPtr = sReceiveBuffer; *pPtr != 0; pPtr++) {
                if (*pPtr == '\n' || *pPtr == '\r') {
                    *pPtr = 0;
                    break;
                }
            }
            // Образать пробелы
            char * prepared_buf  = strtrim(sReceiveBuffer);

            // Пропускаем пустые строки
            if (prepared_buf[0] == 0)
                continue;

            printf("Received data: %s\n", prepared_buf);
            // Анализируем полученные команды или преобразуем текст в верхний регистр
            if (strcmp(prepared_buf, "info") == 0)
            {
                show_info(newsock_fd);
            }
            else if (strcmp(prepared_buf, "status") == 0)
            {
                show_status(newsock_fd);
            }
            else if (strcmp(prepared_buf, "time") == 0)
            {
                show_time(newsock_fd);
            }
            else if (strcmp(prepared_buf, "task") == 0)
            {
                show_task(newsock_fd);
            }
            else if (strcmp(prepared_buf, "exit") == 0)
            {
                send_string(newsock_fd, "Bye!\r\n");
                shutdown(newsock_fd, 2);
                printf("Client initialized disconnection.\r\n");
                break;
            }
            else if (strcmp(prepared_buf, "shutdown") == 0)
            {
                send_string(newsock_fd, "Server shutdown.\r\n");
                sleep(1);
                bTerminate = 1;
                break;
            }
            else if (strncmp(prepared_buf, "factorial ", strlen("factorial ")) == 0)
            {
                factorial(newsock_fd, prepared_buf + strlen("factorial "));
            }
            else
            {
                echo_uppercase(newsock_fd, prepared_buf);
            }
        }
        // Закрываем серверный сокет
        shutdown(server_fd, 2);
        // Освобождаем ресурсы библиотеки сокетов
        return 0;
    }
}

