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

    // Criar conexao socket_A (tambem verifica a resposta do servidor).
    int socket_A;
    if(establish_ftp_connection(settings.ip, SERVER_PORT, &socket_A)){
        exit(-1);
    }
    
    if(login_ftp(socket_A, settings.user, settings.password)){
        exit(-1);
    }

    char* data_ip = malloc(MAX_SIZE);
    int data_port = 0;
    if(enter_ftp_passive_mode(socket_A, data_ip, &data_port)){
        exit(-1);
    }

    printf("data_port = %d\n", data_port);
    printf("data_ip: %s\n", data_ip);

    return 0;
}
