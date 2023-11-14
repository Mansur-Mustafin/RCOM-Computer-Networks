#include "download.h"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
        exit(-1);
    } 

    struct Settings settings;
    memset(&settings, 0, sizeof(settings));

    if(parse_ftp_url(argv[1], &settings)){
        printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
        exit(-1);
    }

    printf("Starting download application\n"
           "  - User: %s\n"
           "  - Password: %s\n"
           "  - Host: %s\n"
           "  - ULR path: %s\n"
           "  - IP: %s\n",
           settings.user,
           settings.password,
           settings.host,
           settings.url_path,
           settings.ip);

    // Criar conexao socket_A.
    int socket_A;
    if(establish_ftp_connection(settings.ip, SERVER_PORT, &socket_A)){
        exit(-1);
    }

    printf("[INFO] socket_A: %d\n", socket_A);

    // Receive 220 code.
    char* response_buffer = malloc(MAX_SIZE);
    int ftp_response_code = 0;
    if(read_ftp_response(socket_A, response_buffer, &ftp_response_code)){
        free(response_buffer);
        exit(-1);
    }
    printf("[INFO] FTP response code: %d\n", ftp_response_code);

    

    return 0;
}
