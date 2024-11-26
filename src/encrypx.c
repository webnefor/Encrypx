
#include "config.h"


int main(int argc, char *argv[]) {

    uint32_t _result;
    uint64_t sum;
    terminal_args data_of_terminal;
    mainDescriptor _p_files;

    // Terminal data parsing
    data_of_terminal    = parse_args(argc, argv);

    _p_files           = filesHandler(data_of_terminal);

    _p_files.hashSum      = get_hash_sum(data_of_terminal.password);

    _p_files.file_name = data_of_terminal.path;


    if ( _p_files.p_file == NULL )
    {
        perror("FILE *");
        fclose(_p_files.p_trash_file);
        fclose(_p_files.p_file);

        return FAILED;
    }

    //  the hash sum of the password
    sum = get_hash_sum(data_of_terminal.mode);

    //  mode: encrypt or decrypt file
    switch ( sum )
    {
        case ENCRYPT:
            _result = f_encrypt(&_p_files, data_of_terminal.password);
            issueCheck(&_result); // event errors
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
            issueCheck(&_result);
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

