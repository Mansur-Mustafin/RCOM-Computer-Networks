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

#define SERVER_PORT 21

#define h_addr h_addr_list[0]	//The first address in h_addr_list.

struct Settings {
    char user[MAX_SIZE];
    char password[MAX_SIZE];
    char host[MAX_SIZE];
    char url_path[MAX_SIZE];  
    char ip[MAX_SIZE];
};

// Parcer of text (getip.c)
int parse_ftp_url(const char *text, struct Settings *settings);

// Create and connect to socket
int establish_ftp_connection(const char *IP, const int port, int *socketFD);
