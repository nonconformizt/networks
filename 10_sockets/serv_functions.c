#include <unistd.h> 
#include <sys/utsname.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <ctype.h>
#include <time.h>
#include "str_utils.c"


// Функция отсылки текстовой ascii строки клиенту
int send_string(int socket, const char *sString)
{
    return send(socket, sString, strlen(sString), 0);
}

void show_status(int socket) 
{
    struct utsname info;
    uname(&info);
    
    char str[1024];
    sprintf(str, "%s %s\n", info.sysname, info.version);
    send_string(socket, str);
}

void show_task(int socket) 
{
    send_string(socket, "Task: Добавить в сервис поддержку команды, реализующей вычисление факториала\n");
}

void show_info(int socket) 
{
    send_string(socket, "Developed by nonconformizt. \nUse for good.\n");
}

void show_time(int socket) 
{
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    char buf[1024];
    sprintf(buf, "System time: %02d/%02d/%d %02d:%02d\n", 
        timeinfo->tm_mday, 
        timeinfo->tm_mon, 
        (timeinfo->tm_year + 1900), 
        timeinfo->tm_hour, 
        timeinfo->tm_min
    );
    send_string(socket, buf);
}

void echo_uppercase(int socket, char * str)
{
    // Преобразовываем строку в верхний регистр
    char sSendBuffer[1024];
    snprintf(sSendBuffer, sizeof(sSendBuffer), "Server reply: %s\r\n", strupr(str));
    send_string(socket, sSendBuffer);
}

void factorial(int socket, char * str) 
{
    int num = atoi(str);

    int f = 1;
    while (num > 1) {
        f *= num;
        num--;
    }

    char res[128];
    strcat(res, "= ");
    strcat(res, itoa(f, 10));
    strcat(res, "\n");
    send_string(socket, res);
}

void prompt(int socket)
{
    send_string(socket, "> ");
}