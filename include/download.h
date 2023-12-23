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
#define NOT_CONNECTED -1 // while socket are not connected

#define h_addr h_addr_list[0]	// The first address in h_addr_list.

#define CODE_220 220 // Response code for "Service ready for new user."
#define CODE_331 331 // Response code for "User name okay, need password."
#define CODE_230 230 // Response code for "User logged in, proceed."
#define CODE_227 227 // Response code for "Entering Passive Mode (h1,h2,h3,h4,p1,p2)."
#define CODE_150 150 // Response code for "File status okay; about to open data connection."
#define CODE_125 125 // Response code for "Data connection already open." 
#define CODE_226 226 // Response code for "Closing data connection."
#define CODE_221 221 // Response code for "Service closing control connection."

struct Settings {
    char user[MAX_SIZE];
    char password[MAX_SIZE];
    char host[MAX_SIZE];
    char host_name[MAX_SIZE];   // oficial host name.
    char url_path[MAX_SIZE];  
    char ip[MAX_SIZE];
    char filename[MAX_SIZE];
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

// send command pasv and cacl the data_port
int enter_ftp_passive_mode(const int socket_fd, char* data_ip, int* data_port);

// Baixa o arquivo.
int download_file(const int socket_fd_A, const int socket_fd_B, const char* url_path, const char* filename);

// Fecha a conexao do A, e B se foram abertas
int clouse_connection(const int socket_fd_A, const int socket_fd_B);

// Funcao auxiliar que faz free()
void free_resources(char* buf1, char* buf2, char* buf3);
