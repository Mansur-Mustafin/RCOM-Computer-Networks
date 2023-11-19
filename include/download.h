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
#define CODE_150 150
#define CODE_226 226

struct Settings {
    char user[MAX_SIZE];
    char password[MAX_SIZE];
    char host[MAX_SIZE];
    char host_name[MAX_SIZE]; // of. host name.
    char url_path[MAX_SIZE];  
    char ip[MAX_SIZE];
};

enum state{
    START, 
    CODE,   // state of receive code
    MESSAGE,   // state of receive message
    FEUP_MOMENT, // case quando pqp tem 200-text
    STOP
};

// Parcer of text (getip.c)
int parse_ftp_url(const char *text, struct Settings *settings);

// Create socket and connect it
int connect_socket(const char *IP, const int port, int *socket_fd);

// Create socket and connect it and verify response the server
int establish_ftp_connection(const char *IP, const int port, int *socket_fd);

// Leia de socket_fd o response_code e response_message
int read_ftp_response(const int socket_fd, char* response_message, int* response_code);

// Envia commando ao servidor
int send_ftp_command(const int socket_fd, const char* command);

// Faz Login ao servidor (USER anonymous; PASS anonymous) 
int login_ftp(const int socket_fd, const char* username, const char* password);

// TODO: perguntar ao professor se e nessario pegar o data_ip? Ele nao vai ser igual ao IP do socket_A?
// send command pasv and cacl the data_port
int enter_ftp_passive_mode(const int socket_fd, char* data_ip, int* data_port);

// Baixa o arquivo.
int download_file(const int socket_fd_A, const int socket_fd_B, const char* url_path);

// 
int clouse_connection(const int socket_fd);

// Funca auxiliar que faz free()
void free_resources(char* buf1, char* buf2, char* buf3);
