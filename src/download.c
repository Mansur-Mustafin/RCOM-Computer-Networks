#include "download.h"

int parse_ftp_url(const char *text, struct Settings *settings) {

    // Check if URL contains user and password info
    const char *at_sign = strchr(text, '@');
    int result;
    if (at_sign) {
        // URL contains user and password
        result = sscanf(text, "ftp://%255[^:]:%255[^@]@%255[^/]/%255[^\n]",
                                settings->user,
                                settings->password,
                                settings->host,
                                settings->url_path);

        if (result < 3) return -1;
        if (result == 3) strncpy(settings->url_path, "/", MAX_SIZE - 1); // Se nao tiver o URL-path
        
    } else {
        // URL does not contain user and password, parse accordingly
        result = sscanf(text, "ftp://%255[^/]/%255[^\n]", 
                                settings->host, 
                                settings->url_path);
        
        if (result < 1) return -1;
        if (result == 1) strncpy(settings->url_path, "/", MAX_SIZE - 1); // Se nao tiver o URL-path
        
        strncpy(settings->user, "anonymous", MAX_SIZE - 1);
        strncpy(settings->password, "anonymous", MAX_SIZE - 1);
    }

    if (strlen(settings->host) == 0) return -1;
    struct hostent *h;
    if ((h = gethostbyname(settings->host)) == NULL) {
        herror("gethostbyname()");
        return -1;
    }

    strncpy(settings->ip, inet_ntoa(*((struct in_addr *) h->h_addr)), MAX_SIZE - 1);

    return 0;
}

int establish_ftp_connection(const char *IP, const int port, int *socketFD){

    int sockfd;
    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP);    /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(port);        /*server TCP port must be network byte ordered */

    /*open a TCP socket*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }
    /*connect to the server*/
    if (connect(sockfd,
                (struct sockaddr *) &server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect()");
        return -1;
    }

    *socketFD = sockfd;

    return 0;
}
