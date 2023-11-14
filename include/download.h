#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


#define TRUE 1
#define FALSE 0

#define MAX_SIZE 256
#define MAX_RESPONSE_SIZE 1024

#define SERVER_PORT 21 // TPF tranfer

#define h_addr h_addr_list[0]	// The first address in h_addr_list.

#define CODE_220 220
#define CODE_331 331
#define CODE_230 230
#define CODE_227 227

struct Settings {
    char user[MAX_SIZE];
    char password[MAX_SIZE];
    char host[MAX_SIZE];
    char url_path[MAX_SIZE];  
    char ip[MAX_SIZE];
};

enum state{
    START, 
    CODE,   // state of receive code
    MESSAGE,   // state of receive message
    STOP
};

// Parcer of text (getip.c)
int parse_ftp_url(const char *text, struct Settings *settings);

// Create and connect to socket
int establish_ftp_connection(const char *IP, const int port, int *socketFD);

// Leia de socket_fd o response_code e response_message
int read_ftp_response(const int socket_fd, char* response_message, int* response_code);

// Envia commando ao servidor
int send_ftp_command(const int socket_fd, const char* command);

// Faz Login ao servidor (USER anonymous; PASS anonymous) 
int login_ftp(const int socket_fd, const char* username, const char* password);