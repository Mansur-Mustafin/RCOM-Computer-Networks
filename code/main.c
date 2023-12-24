#include "download.h"
#include <sys/time.h>
#include <time.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
        exit(-1);
    } 

    struct Settings settings;
    memset(&settings, 0, sizeof(settings));
    struct timeval start, end;
    double time_used;

    if(parse_ftp_url(argv[1], &settings)){
        printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
        exit(-1);
    }

    printf("Starting download application\n"
           "  - User: %s\n"
           "  - Password: %s\n"
           "  - Host: %s\n"
           "  - Host name: %s\n"
           "  - ULR path: %s\n"
           "  - IP: %s\n",
           settings.user,
           settings.password,
           settings.host,
           settings.host_name,
           settings.url_path,
           settings.ip);

    /*Criar socket A, connectar ele, e verificar resposta do servidor*/
    int socket_A;
    if(establish_ftp_connection(settings.ip, SERVER_PORT, &socket_A)){
        exit(-1);
    }
    printf("[INFO] socket_A: %d\n", socket_A);

    if(login_ftp(socket_A, settings.user, settings.password)){
        clouse_connection(socket_A, NOT_CONNECTED);
        exit(-1);
    }

    char* data_ip = malloc(MAX_SIZE);       // ip to connect socket_B
    int data_port = 0;                      // port to connect socket_B
    if(enter_ftp_passive_mode(socket_A, data_ip, &data_port)){
        clouse_connection(socket_A, NOT_CONNECTED);
        exit(-1);
    }

    int socket_B;
    if(connect_socket(data_ip, data_port, &socket_B)){
        clouse_connection(socket_A, NOT_CONNECTED);
        exit(-1);
    }
    printf("[INFO] socket_B: %d\n", socket_B);
    
    if(gettimeofday(&start, NULL)) {
        perror("gettimeofday");
        if(clouse_connection(socket_A, socket_B)) exit(-1);
        exit(-1);
    }

    if(download_file(socket_A, socket_B, settings.url_path, settings.filename)){
        if(clouse_connection(socket_A, socket_B)) exit(-1);
        exit(-1);
    }

    if(gettimeofday(&end, NULL)) {
        perror("gettimeofday");
        if(clouse_connection(socket_A, socket_B)) exit(-1);
        exit(-1);
    }

    if(clouse_connection(socket_A, socket_B)) exit(-1);

    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    printf("Download %s completed in %.2f seconds\n", settings.filename, time_used);

    return 0;
}
