#include "download.h"


int get_input(const char *text, struct Settings *settings) {

    // Attempt to scan with user and password
    int result = sscanf(text, "ftp://%255[^:]:%255[^@]@%255[^/]/%255[^\n]",
                                settings->user, 
                                settings->password, 
                                settings->host, 
                                settings->url_path);

    if(result != 4){
        return -1;
    }

    return 0;
}


int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
        exit(-1);
    } 

    struct Settings settings;
    memset(&settings, 0, sizeof(settings));

    if(get_input(argv[1], &settings)){
        printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
        exit(-1);
    }

    printf("Starting download application\n"
           "  - User: %s\n"
           "  - Password: %s\n"
           "  - Host: %s\n"
           "  - ULR Path: %s\n",
           settings.user,
           settings.password,
           settings.host,
           settings.url_path);

    return 0;
}