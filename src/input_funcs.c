
#include "config.h"


terminal_args parse_args(int argc, char *argv[]) {
/*
    * The function processes the following command-line options:
    * - -h: Displays help information and exits.
    * - -f: Specifies the file path.
    * - -k: Specifies the encryption/decryption password.
    * - -m: Specifies the mode (encrypt or decrypt).
    * - -t: Specifies the trash file path.
    * - -s: Parameter for speed (currently in development).
    *
    * The function checks for the presence of required arguments (-f, -k, -m, -t) and exits with an error if any are missing.
    * It also checks the password strength and emits a warning if the password is too weak.
    *
    * If no arguments are provided, or if any required arguments are missing, the function exits and prints an appropriate message.
    *
    * @note The -s option is currently under development and does not perform any actions.
 */

    terminal_args terminalArgs;
    signed char TempData[64];
    int result_code; // result code

    for (int i = 0; i < argc; i++)
    {

        if (argc < 2)
        {
            puts(
                "encrypx: bad usage\n"
                "Try 'encrypx -h' for more information"                
                 );

            exit(-1);
        }

        if (strcmp(argv[i], HELP) == 0)
        {
            show_help();
            exit(0);
        }

        if (strcmp(argv[i], VERSION) == 0)
        {
            puts("v" RELEASE);
            exit(0);
        }
        
        if (strcmp(argv[i], FILE) == 0) {
            if ((result_code = !isEmpty(argv[i + 1])))
                terminalArgs.path = argv[i + 1];
        }

        if (strcmp(argv[i], KEY) == 0) {
            if ((result_code = !isEmpty(argv[i + 1]))) {
                terminalArgs.password = argv[i + 1];
            }
        }

        if (strcmp(argv[i], MODE) == 0) {
            if ((result_code = !isEmpty(argv[i + 1]))) {
                terminalArgs.mode = argv[i + 1];
            }
        }
        if (strcmp(argv[i], TRASH) == 0) {
            if ((result_code = !isEmpty(argv[i + 1]))) {
                terminalArgs.trash = argv[i + 1];
            }
        }

        if (strcmp(argv[i], SPEED) == 0) {
            if ((result_code = !isEmpty(argv[i + 1])));
                // in developing
        }
    }

    if (terminalArgs.path == NULL || terminalArgs.password == NULL || terminalArgs.mode == NULL || terminalArgs.trash == NULL) {
        puts("encrypx: missing required arguments");
        exit(-1);
    }

    if (strlen(terminalArgs.password) < 16)
        puts(ORANGE "[WARNING]" " " DEFAULT "Your password is too weak");

    return terminalArgs;
}


mainDescriptor filesHandler(terminal_args PathTo) {

    mainDescriptor  filesDescriptor;

    filesDescriptor.p_file         = fopen(PathTo.path, RAW);

    if (filesDescriptor.p_file == NULL)
    {
       perror(RED "[-f] Wait");
       puts(DEFAULT"Try -h to show help");
       exit(FAILED);
    }

    filesDescriptor.p_trash_file   = fopen(PathTo.trash, RBW);

    if (filesDescriptor.p_trash_file == NULL)
    {
       perror(RED "[-t] Wait");
       puts(DEFAULT"Try -h to show help");
       exit(FAILED);
    }

   return filesDescriptor;
}
 

uint32_t issueCheck (const unsigned int *result ) {

    if (result < 0) {
        perror("");
        return FAILED;
    };

    return OK;
}

bool isEmpty(char *str) {

    /*
     * Checks if a given string is empty or NULL.
     * @return Returns 1 (true) if the string is NULL or empty, otherwise returns 0 (false).
    */

    if (str == NULL || *str == '\0')
        return 0;

    char *reserved[] = {TRASH, MODE, FILE, KEY};

    for (int i = 0; i < (sizeof(reserved) / 8); i++) {
        if (strcmp(str, reserved[i]) == 0)
        {
            printf(RED "[-]" " " DEFAULT "Using a reserved value\n");
            return -1;
        }
    }
}


void show_help() {

    printf  (   "Options:\n"                                                           );
    printf  (   "\t-f, --file \tMain file for encrypting\n"                            );
    printf  (   "\t-t, --trash\tThe trash file [in developing]\n"                      );
    printf  (   "\t-k, --key\tYour password for encrypting\n"                          );
    printf  (   "\t-m, --mode\tMode of regime,has the encrypt & decrypt\n"             );
    printf  (   "\t-h, --help\tdisplay this help and exit\n"                           );
    printf  (   "\t-v, --version\tdisplay version and exit\n"                          );
    printf  (   "\n"                                                                   );
    printf  (   "\tExample: ./encrypx -f date.* -t trash.data* -k qwerty -m encrypt\n" );
    printf  (   "\n"                                                                   );

    // see ya
}
