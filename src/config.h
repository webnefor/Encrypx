
// this is value for crypting

#include <sys/_types/_u_int64_t.h>
#define FIND_HASH "0xFFXX" // Hash of the file =

#define C_XOR_1 0xcc9e2d51
#define C_XOR_2 0x1b873593
#define C_XOR_3 0xe6546b64
#define C_XOR_4 0x85ebca6b
#define C_XOR_5 0xc2b2ae35


// the regimes
#define DECRYPT   0x63628d49
#define ENCRYPT   0x7dbdabcc

// the value if there's some errors
#define FAILED  -0x01
#define OK       0x0
 

#define SLOW      2048    
#define HIGH      8196
#define FASTER    8196 * 10

#define BUFFER_SIZE FASTER


#define RAWB "r+a+w+b"
#define RBW  "rb+w"
#define TEMP_TRASH ".trash"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>



typedef struct of_args {

    /*
     * Args of the terminal *
    */

    char    *   path    ;
    char    *   password;
    char    *   mode    ;
    char    *   trash   ;
    char    *   help    ;

} t_args; 


typedef struct of_dsc {

     /*
      * Descriptor for changing the files *
     */

    FILE        *   p_file      ;
    FILE        *   p_trash_file;
    u_int64_t       hsum        ;
    u_int64_t   *   p_elmn      ;
    char        *   file_name   ;  
    
} desc;


extern t_args       parse_args      ( int argc, char *argv[]         );
extern desc         file_handler    ( t_args p_to_path               );

extern uint32_t     f_encrypt       ( desc * p_files__ , char *pssw  );
extern uint32_t     f_decrypt       ( desc * p_files__ , char *pssw  );

extern uint64_t     hsum            ( char * pssw                    );
extern uint32_t     h_error         ( unsigned int *p_result         );

extern u_int64_t    set_size_of_file(desc*                           );

extern unsigned int import_hsum     ( desc *                         );
extern unsigned int check_hsum      ( desc *                         );

extern void         progress_bar    (int i                           );

extern void         show_help       ();


#define     FILE    "-f"
#define     KEY     "-k"
#define     MODE    "-m"
#define     TRASH   "-t"
#define     SPEED   "-s"
#define     HELP    "-h"   
