
#include "config.h"



t_args parse_args(int argc, char *argv[]) {

    #define     FILE    "-f"
    #define     KEY     "-k"
    #define     MODE    "-m"
    #define     TRASH   "-t"
    #define     HELP    "-h"

    t_args temp_data;
    signed char _data[64];



    for (int i = 0; i < argc; i++) {

        if (strcmp(argv[i], HELP) == 0)
        {
            show_help();
            exit(0);
        }

        if (strcmp(argv[i], FILE) == 0)
            temp_data.path  = argv[i + 1];

        if (strcmp(argv[i], KEY) == 0)
            temp_data.password       = argv[i+1];

        if (strcmp(argv[i], MODE) == 0)
            temp_data.mode       = argv[i+1];

        if (strcmp(argv[i], TRASH) == 0)
            temp_data.trash      = argv[i+1];

    }
    
    
    
    //   if (strlen(temp_data.password) > 16) {
    //     puts("the password must has 128 bite [16 symbols]\n");
    //     printf("lenght of your password is: %s", strlen(temp_data.password));
    //     puts("\n");
    //     exit(FAILED);
    // }

    return temp_data;
}


desc file_handler( t_args p_to_path ) {

    desc  __p_files_descriptor;

    __p_files_descriptor.p_file         = fopen(p_to_path.path, "r+a+w+b");

    __p_files_descriptor.p_trash_file   = fopen(p_to_path.trash, "rb+w");

    if ( __p_files_descriptor.p_file == NULL ) 
    {
       perror("Ошибка при открытии файла");
       fclose(__p_files_descriptor.p_file);
       exit(-1);
    }

    if ( __p_files_descriptor.p_trash_file == NULL ) 
    {
       perror("Ошибка при открытии файла");
       fclose(__p_files_descriptor.p_file);
       exit(-1);
    }

   return __p_files_descriptor;
}
 

uint32_t h_error ( unsigned int *p_result ) {

    if (p_result < 0) {
        perror("");
        return FAILED;
    };

    return OK;
}


void show_help() {

    printf  ("\n");
    printf  ("Options:\n"                                              );
    printf  ("\t-f\tMain file for crypting\n"                          );
    printf  ("\t-t\tThe trash file [in developing]\n"                  );
    printf  ("\t-k\tYour password for crypting\n"                      );
    printf  ("\t-m\tRegime of mode, can crypt or encrypt\n"            );
    printf  ("\t\t[The regime has two params, like: encrypt & decrypt]");
    printf  ("\n");

    printf ("\n");

}