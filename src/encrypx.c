
#include "config.h"


int main(int argc, char *argv[]) {

    uint32_t _result;
    uint64_t sum;
    t_args data_of_terminal;
    desc _p_files;

    // Terminal data parsing
    data_of_terminal    = parse_args(argc, argv);

    _p_files           = file_handler(data_of_terminal);

    _p_files.hsum      = hsum(data_of_terminal.password);

    _p_files.file_name = data_of_terminal.path;


    if ( _p_files.p_file == NULL )
    {
        perror("FILE *");
        fclose(_p_files.p_trash_file);
        fclose(_p_files.p_file);

        return FAILED;
    }

    //  the hash sum of the password
    sum = hsum(data_of_terminal.mode);

    //  mode: encrypt or decrypt file
    switch ( sum )
    {
        case ENCRYPT:
            _result = f_encrypt(&_p_files, data_of_terminal.password);
            h_error(&_result); // event errors
            break;

        case DECRYPT:
            if (!validateHash(&_p_files))
             // check true or false the hash of the file.
              _result = f_decrypt(&_p_files, data_of_terminal.password);
            else
            {
                printf(RED "[-]" DEFAULT " " "The key ain't correct!\n");
                exit(0);
            }
            h_error(&_result);
            break;

        default:
            printf(RED "encrypx: " "Can't find the command (%s)\n", data_of_terminal.mode);
            printf(DEFAULT "\tTry -h to see more\n");
            fclose(_p_files.p_file);
            fclose(_p_files.p_trash_file);
            return FAILED;
    }

    printf("\n" GREEN "[+]" DEFAULT " " "Success\n");

    return OK;
}

