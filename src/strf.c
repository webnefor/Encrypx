
#include "config.h"

t_args parse_args(int argc, char *argv[]) {

// #define     FILE    "-f"
// #define     KEY     "-k"
// #define     MODE    "-m"
// #define     TRASH   "-t"
// #define     SPEED   "-s"
// #define     HELP    "-h"   

    t_args temp_data;
    signed char _data[64];

    for (int i = 0; i < argc; i++)
    {

        if (argc < 2)
        {
            puts(
                    "[nefor] Cannot find any params.\n"
                    "\t-h\tto show help"
                 );
            exit(-1);
        }

        if (strcmp(argv[i], HELP) == 0)
        {
            show_help();
            exit(0);
        }

        if (strcmp(argv[i], FILE) == 0)
            temp_data.path              = argv[i+1];

        if (strcmp(argv[i], KEY) == 0)
            temp_data.password          = argv[i+1];

        if (strcmp(argv[i], MODE) == 0)
            temp_data.mode              = argv[i+1];

        if (strcmp(argv[i], TRASH) == 0)
            temp_data.trash             = argv[i+1];

        if (strcmp(argv[i], SPEED) == 0)
        {
            // in developing
            exit(0);
        }
    }

    if (strlen(temp_data.password) < 16)
        puts("[WARNING] Your password is too weak");


    return temp_data;
}


desc file_handler( t_args p_to_path ) {

    desc  __p_files_descriptor;

    __p_files_descriptor.p_file         = fopen(p_to_path.path, RAWB);

    __p_files_descriptor.p_trash_file   = fopen(p_to_path.trash, RBW);

    if ( __p_files_descriptor.p_file == NULL ) 
    {
       perror("[Cannot open the file]");
       puts("Try -h to show help");
       fclose(__p_files_descriptor.p_file);
       exit(-1);
    }

    if ( __p_files_descriptor.p_trash_file == NULL ) 
    {
       perror("[Cannot open the trash file]");
       puts("Try -h to show help");
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
    printf  ("Options:\n"                                                   );
    printf  ("\t-f\tMain file for encrypting\n"                             );
    printf  ("\t-t\tThe trash file [in developing]\n"                       );
    printf  ("\t-k\tYour password for encrypting\n"                         );
    printf  ("\t-m\tMode of regime, can encrypt or decrypt\n"               );
    printf  ("\t\t[The regime has two params, like: encrypt & decrypt]"     );
    printf  ("\n");
    // see ya
    printf ("\n");

}
