
#include "config.h"


int main(int argc, char *argv[]) {
    
    uint32_t __result;
    uint64_t sum;

    // Terminal data parsing

    t_args data_of_terminal;

    desc __p_files;

    data_of_terminal    = parse_args(argc, argv);

    __p_files           = file_handler(data_of_terminal);
    
    __p_files.hsum      = hsum(data_of_terminal.password);

    __p_files.file_name = data_of_terminal.path;

    if ( __p_files.p_file == NULL ) 
    {
        perror("FILE *");
        fclose(__p_files.p_trash_file);
        fclose(__p_files.p_file);
        
        return FAILED;
    }

    //  the hash sum of the password
    sum = hsum(data_of_terminal.mode);

    //  mode: encrypt or decrypt file
    switch ( sum ) 
    {   
        case ENCRYPT:
            __result = f_encrypt(&__p_files, data_of_terminal.password);
            h_error(&__result); // event errors

            break;

        case DECRYPT:
            if (!check_hsum(&__p_files))  // check true or false of the hash of the file
                __result = f_decrypt(&__p_files, data_of_terminal.password);
            else 
            {
                printf("[Password]: ain't correct!\n");
                exit(0);
            } 
            h_error(&__result);

            break;

        default:
            printf("[-] Can't find [%s]\nPlease, try -h\n", data_of_terminal.mode);
            fclose(__p_files.p_file);
            fclose(__p_files.p_trash_file);
            
            return FAILED;
    }

    printf("[+] Done\n");
    return 0;
}

