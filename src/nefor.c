
#include "config.h"
#include <stdio.h>

//
// Что я хочу?
//
// Чтобы из двух файлов, создавался один и шифровались
// В зависимости от пароля, должно выдаваться данные
// зависящие от этого пароля, а другие должны уничтожаться 
// "эдакий план Б"
//
// 1) Для начала зашифровать файл главный
//
// 2) Потом второй
//
// 3) 
//

// nefor -f main.txt -t trash.txt -k 12345


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
            __result = encrypter(&__p_files, data_of_terminal.password);
            h_error(&__result); // event errors

            break;

        case DECRYPT:
            if (!check_hsum(&__p_files))  // check true or false of the hash of the file
                __result = decrypter(&__p_files, data_of_terminal.password);
            else 
            {
                printf("[PASSWORD]: it ain't correct!\n");
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

    return 0;
}

