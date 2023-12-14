#include "download.h"

int parse_ftp_url(const char *text, struct Settings *settings){

    // Check if URL contains user and password info
    const char *at_sign = strchr(text, '@');
    int result;
    if(at_sign){
        // URL contains user and possibly a password
        char user_pass[512];
        result = sscanf(text, "ftp://%511[^@]@%255[^/]/%255[^\n]", 
                               user_pass,
                               settings->host, 
                               settings->url_path);

        if (result != 3) return -1;

        char *colon = strchr(user_pass, ':');
        if (colon) {
            *colon = '\0';
            strncpy(settings->user, user_pass, MAX_SIZE - 1);
            strncpy(settings->password, colon + 1, MAX_SIZE - 1);
        } else {
            return -1;
        }
    } else {
        // URL does not contain user and password, parse accordingly
        result = sscanf(text, "ftp://%255[^/]/%255[^\n]", 
                                settings->host, 
                                settings->url_path);
        
        if (result != 2) return -1;
        
        strncpy(settings->user, "anonymous", MAX_SIZE - 1);
        strncpy(settings->password, "anonymous", MAX_SIZE - 1);
    }

    if(strlen(settings->host) == 0) return -1;
    struct hostent *h;
    if((h = gethostbyname(settings->host)) == NULL){
        herror("gethostbyname()");
        return -1;
    }

    /*save the ip*/
    strncpy(settings->ip, inet_ntoa(*((struct in_addr *) h->h_addr)), MAX_SIZE - 1);
    strncpy(settings->host_name, h->h_name, MAX_SIZE - 1);

    return EXIT_SUCCESS;
}

int connect_socket(const char *IP, const int port, int *socket_fd){
    if(IP == NULL || socket_fd == NULL) return -1;

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

    return EXIT_SUCCESS;
}

int establish_ftp_connection(const char *IP, const int port, int *socket_fd){

    if(connect_socket(IP, port, socket_fd)) return -1;

    /*read response from server*/
    char* response_buffer = malloc(MAX_RESPONSE_SIZE);
    int ftp_response_code = 0;

    if(read_ftp_response((*socket_fd), response_buffer, &ftp_response_code)){
        printf("ERROR failed read response message\n");
        free(response_buffer);
        return -1;
    }

    if(ftp_response_code != CODE_220){
        printf("[ERROR] failed onnection to %s\n", IP);
        return -1;
    }
    free(response_buffer);
    return EXIT_SUCCESS;
}

int read_ftp_response(const int socket_fd, char* response_buffer, int* response_code){
    if(response_buffer == NULL || response_code == NULL) return -1;

    enum state state = START;
    int indx = 0;
    *response_code = 0;
    int prev_code = 0;

    while(state != STOP){
        char byte = 0;
        int read_status = read(socket_fd, &byte, sizeof(byte));

        if(read_status == -1) return -1;
        
        // printf("flag: %d; byte: %d; byte: %c\n", read_status, byte, byte);

        switch (state)
        {
        case START:
            state = CODE;
        case CODE:
            if(read_status == 0 || byte == '\n') state = STOP;
            else if(read_status == 1 && (byte >= 48 && byte <= 57)){    // case if is digit
                *response_code += byte - 48;    // Transform ASCII to decimal number
                *response_code *= 10;           // Left shift 
            }
            else if(read_status == 1 && byte == ' ') state = MESSAGE;
            else if(read_status == 1 && byte == '-') state = FEUP_MOMENT;
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
        case FEUP_MOMENT:
            if(byte == '\n'){
                state = CODE;
                if(prev_code != 0 && prev_code != *response_code) return -1; // On new line not the same code.
                
                prev_code = *response_code;
                *response_code = 0; 
            }
            response_buffer[indx++] = byte;
            break;
        default:
            state = START;
            break;
        }
    }
    
    *response_code /= 10;

    printf("[INFO] [%d] ", *response_code);
    printf("Message:\n%s\n\n", response_buffer);
    return EXIT_SUCCESS;
}

int send_ftp_command(const int socket_fd, const char* command){
    size_t bytes = write(socket_fd, command, strlen(command));
    if(bytes < 0){
        perror("[ERROR] filed sending command to FTP server\n");
        return -1;
    }
    return EXIT_SUCCESS;
}

int login_ftp(const int socket_fd, const char* username, const char* password){
    if(username == NULL || password == NULL) return -1;

    char *command = malloc(MAX_RESPONSE_SIZE);
    char *response = malloc(MAX_RESPONSE_SIZE);
    int response_code = 0;

    // Send USER command
    snprintf(command, MAX_RESPONSE_SIZE, "USER %s\r\n", username);
    if(send_ftp_command(socket_fd, command)){
        free_resources(response, command, NULL);
        return -1;
    }
    
    if(read_ftp_response(socket_fd, response, &response_code)){
        free_resources(response, command, NULL);
        return -1;
    } 
    
    if(response_code != CODE_331){
        printf("[ERROR] login failed with user: %s\n", username);
        free_resources(response, command, NULL);
        return -1;
    }

    // Send PASS command
    snprintf(command, MAX_RESPONSE_SIZE, "PASS %s\r\n", password);
    if(send_ftp_command(socket_fd, command)){
        free_resources(response, command, NULL);
        return -1;
    }

    if(read_ftp_response(socket_fd, response, &response_code)){
        free_resources(response, command, NULL);
        return -1;
    }

    if(response_code != CODE_230){
        printf("[ERROR] login failed with password: %s\n", password);
        free_resources(response, command, NULL);
        return -1;
    }

    free_resources(response, command, NULL);

    // printf("[INFO] FTP login successful\n");
    return EXIT_SUCCESS;
}

int enter_ftp_passive_mode(const int socket_fd, char* data_ip, int* data_port){
    if(data_ip == NULL || data_port == NULL) return -1;

    char *pasv_command = "pasv\r\n";
    char *response = malloc(MAX_RESPONSE_SIZE);
    int response_code = 0;

    if(send_ftp_command(socket_fd, pasv_command)) {
        free(response);
        return -1;
    }

    if(read_ftp_response(socket_fd, response, &response_code)){
        free(response);
        return -1;
    } 

    if(response_code != CODE_227){
        printf("[ERROR] failed pasv command\n");
        free(response);
        return -1;
    }
    
    int ip1, ip2, ip3, ip4, port1, port2;
    int result = sscanf(response, 
                        "Entering Passive Mode (%d,%d,%d,%d,%d,%d)", 
                            &ip1, &ip2, &ip3, &ip4, &port1, &port2);

    if(result != 6){
        printf("[ERROR] response pasv\n");
        return -1;
    }

    snprintf(data_ip, MAX_SIZE, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);

    *data_port = (port1 << 8) + port2;  // port1 * 256 + port2
    free(response);
    return EXIT_SUCCESS;
}

int download_file(const int socket_fd_A, const int socket_fd_B, const char* url_path){

    char *command = malloc(MAX_RESPONSE_SIZE);
    char *response = malloc(MAX_RESPONSE_SIZE);
    int response_code = 0;

    // Send retr command.
    snprintf(command, MAX_RESPONSE_SIZE, "retr %s\r\n", url_path);
    if(send_ftp_command(socket_fd_A, command) < 0){
        free_resources(response, command, NULL);
        return -1;
    }
    
    if(read_ftp_response(socket_fd_A, response, &response_code) < 0){
        free_resources(response, command, NULL);
        return -1;
    } 
    
    /*if(response_code != CODE_150 && response_code != CODE_125){
        printf("[ERROR] retr command with URL path: %s\n", url_path);
        free_resources(response, command, NULL);
        return -1;
    }*/
    
    if(response_code / 100 != 1){
        printf("[ERROR] retr command with URL path: %s\n", url_path);
        free_resources(response, command, NULL);
        return -1;
    }

    // Download file.

    char filename[MAX_SIZE];
    const char *last_slash = strrchr(url_path, '/');
    if (last_slash != NULL) {
        strncpy(filename, last_slash + 1, MAX_SIZE - 1);
    } else {
        strncpy(filename, url_path, MAX_SIZE - 1);
    }
    filename[MAX_SIZE - 1] = '\0';


    FILE *file = fopen(filename, "wb");

    if(file == NULL){
        printf("[ERROR] open file\n");
        return -1;
    }

    char *buf = malloc(MAX_SIZE);
    int bytes_read = 0;

    while( (bytes_read = read(socket_fd_B, buf, MAX_SIZE)) > 0 ){
        if(fwrite(buf, bytes_read, 1, file) < 0){
            free_resources(buf, response, command);
            return -1;
        }
    }
    fclose(file);

    // Check if its was finished.

    do
        {
            if(read_ftp_response(socket_fd_A, response, &response_code) < 0){
            free_resources(buf, response, command);
            return -1;
        } 

        if(response_code != CODE_226){
            printf("[ERROR] transfer was not complete\n");
            free_resources(buf, response, command);
            return -1;
        }
    } while (response_code / 100 < 2);


    

    free_resources(buf, response, command);
    return EXIT_SUCCESS;
}

int clouse_connection(const int socket_fd_A, const int socket_fd_B){

    char *pasv_command = "quit\r\n";
    char *response = malloc(MAX_RESPONSE_SIZE);
    int response_code = 0;

    if(send_ftp_command(socket_fd_A, pasv_command)) {
        free(response);
        return -1;
    }

    if(read_ftp_response(socket_fd_A, response, &response_code)){
        free(response);
        return -1;
    } 

    if(response_code != CODE_221){
        printf("[ERROR] failed quit command\n");
        free(response);
        return -1;
    }

    if (close(socket_fd_A)<0) {
        perror("close()");
        return -1;
    }
    if (socket_fd_B != NOT_CONNECTED && close(socket_fd_B)) {
        perror("close()");
        return -1;
    }
    return EXIT_SUCCESS;
}

void free_resources(char* buf1, char* buf2, char* buf3){
    free(buf1);
    free(buf2);
    free(buf3);
}
