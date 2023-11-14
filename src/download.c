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

int establish_ftp_connection(const char *IP, const int port, int *socket_fd){

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

    *socket_fd = sockfd;

    printf("[INFO] socket_A: %d\n", sockfd);

    /*read response from server*/
    char* response_buffer = malloc(MAX_RESPONSE_SIZE);
    int ftp_response_code = 0;

    if(read_ftp_response(sockfd, response_buffer, &ftp_response_code)){
        printf("ERROR failed read response message\n");
        free(response_buffer);
        return -1;
    }

    if(ftp_response_code != CODE_220){
        printf("[ERROR] failed onnection to %s\n", IP);
        return -1;
    }
    free(response_buffer);
    return 0;
}

int read_ftp_response(const int socket_fd, char* response_buffer, int* response_code){
    if(response_buffer == NULL || response_code == NULL) return -1;

    enum state state = START;
    int indx = 0;
    *response_code = 0;

    while(state != STOP){
        char byte = 0;
        int read_status = read(socket_fd, &byte, sizeof(byte));

        if(read_status == -1) return -1;
        
        // printf("flag: %d byte: %c\n", read_status, byte);

        switch (state)
        {
        case START:
            state = CODE;
        case CODE:
            if(read_status == 0 || byte == '\n') state = STOP;
            else if(read_status == 1 && byte == ' ') state = MESSAGE;
            else if(read_status == 1){
                *response_code += byte - 48;    // Transform ASCII to decimal number
                *response_code *= 10;           // Left shift 
            }
            break;
        case MESSAGE:
            if(read_status == 0 || byte == '\n'){
                state = STOP;
                response_buffer[indx] = '\0';
            } 
            else if(read_status == 1){
                response_buffer[indx++] = byte;
            }
            break;
        default:
            state = START;
            break;
        }
    }
    
    *response_code /= 10;

    printf("[INFO] FTP response code: %d\n", *response_code);
    printf("[INFO] Response message: %s\n", response_buffer);
    return 0;
}

int send_ftp_command(const int socket_fd, const char* command){
    size_t bytes = write(socket_fd, command, strlen(command));
    if (bytes < 0) {
        perror("[ERROR] filed sending command to FTP server");
        return -1;
    }
    return 0;
}

int login_ftp(const int socket_fd, const char* username, const char* password) {
    if(username == NULL || password == NULL) return -1;

    char *command = malloc(MAX_RESPONSE_SIZE);
    char *response = malloc(MAX_RESPONSE_SIZE);
    int response_code = 0;

    // Send USER command
    snprintf(command, MAX_RESPONSE_SIZE, "USER %s\r\n", username);
    if (send_ftp_command(socket_fd, command) < 0) {
        free(response);
        free(command);
        return -1;
    }
    
    if (read_ftp_response(socket_fd, response, &response_code) < 0){
        free(response);
        free(command);
        return -1;
    } 
    
    if (response_code != CODE_331) {
        printf("[ERROR] login failed with user: %s", username);
        free(response);
        free(command);
        return -1;
    }

    // Send PASS command
    snprintf(command, MAX_RESPONSE_SIZE, "PASS %s\r\n", password);
    if (send_ftp_command(socket_fd, command) < 0) {
        free(response);
        free(command);
        return -1;
    }

    if (read_ftp_response(socket_fd, response, &response_code) < 0) {
        free(response);
        free(command);
        return -1;
    }

    if (response_code != CODE_230) {
        printf("[ERROR] login failed with password: %s", password);
        free(response);
        free(command);
        return -1;
    }

    free(response);
    free(command);

    printf("[INFO] FTP login successful\n");
    return 0;
}

