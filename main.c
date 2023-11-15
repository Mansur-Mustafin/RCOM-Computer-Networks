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

    /*Criar socket A, connectar ele, e verificar resposta do servidor*/
    int socket_A;
    if(establish_ftp_connection(settings.ip, SERVER_PORT, &socket_A)){
        exit(-1);
    }
    printf("[INFO] socket_A: %d\n", socket_A);

    if(login_ftp(socket_A, settings.user, settings.password)){
        exit(-1);
    }

    char* data_ip = malloc(MAX_SIZE);       // ip to connect socket_B
    int data_port = 0;                      // port to connect socket_B
    if(enter_ftp_passive_mode(socket_A, data_ip, &data_port)){
        exit(-1);
    }

    int socket_B;
    if(connect_socket(data_ip, data_port, &socket_B)){
        exit(-1);
    }
    printf("[INFO] socket_B: %d\n", socket_B);
    
    // TODO: temos que ler algo depois de conectar o socket? (Eu verifiquei tem nada la).

    if(download_file(socket_A, socket_B, settings.url_path)){
        exit(-1);
    }

    // TODO: so isso?
    if(clouse_connection(socket_A) || clouse_connection(socket_B)){
        exit(-1);
    }

    return 0;
}
