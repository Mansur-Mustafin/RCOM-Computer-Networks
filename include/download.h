#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

#define MAX_SIZE 256

// Here we can define all possible urls
#define INIT_URL "ftp://%255[^:]:%255[^@]@%255[^/]/%255[^\n]"

struct Settings {
    char user[MAX_SIZE];
    char password[MAX_SIZE];
    char host[MAX_SIZE];
    char url_path[MAX_SIZE];  
};

// Parcer of text
int get_input(const char *text, struct Settings *settings);

