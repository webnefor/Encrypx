
#include "config.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint64_t move_from = 0;
// static uint64_t len = 0;
u_int64_t hsum(char *password) {
    
    uint32_t len = strlen(password);

    uint32_t c1 = C_XOR_1;
    uint32_t c2 = C_XOR_2;
    uint32_t r1 = 15;
    uint32_t r2 = 13;
    uint32_t m = 5;
    uint32_t n = C_XOR_3;
    uint32_t hash = len * 5;

    const int nblocks = len / 4;
    const uint32_t *blocks = (const uint32_t *)password;
    int i;
    for (i = 0; i < nblocks; i++) {
        uint32_t k = blocks[i];
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        hash ^= k;
        hash = (hash << r2) | (hash >> (32 - r2));
        hash = hash * m + n;
    }

    const uint8_t *tail = (const uint8_t *)(password + nblocks * 4);
    uint32_t k1 = 0;

    switch (len & 3) {
        case 3:
            k1 ^= tail[2] << 16;
        case 2:
            k1 ^= tail[1] << 8;
        case 1:
            k1 ^= tail[0];
            k1 *= c1;
            k1 = (k1 << r1) | (k1 >> (32 - r1));
            k1 *= c2;
            hash ^= k1;
    }

    hash ^= len;
    hash ^= (hash >> 16);
    hash *= C_XOR_4;
    hash ^= (hash >> 13);
    hash *= C_XOR_5;
    hash ^= (hash >> 16);

    return abs(hash);
}

uint32_t decrypter(desc * __pdescr, char *password) {

    char c;
    FILE *trash = fopen(TEMP_TRASH, "wb+a+r");

    // char a = 's';
    fseek(__pdescr->p_file, 0, SEEK_SET);
    fseek(trash, 0, SEEK_SET);

    char __hash[254];
    snprintf(__hash, sizeof(__hash), "%d", __pdescr->hsum);
    off_t key_len = strlen(password);
    for (int i = 0; i < (move_from - strlen(__hash)-6); i++) 
    {
        c = fgetc(__pdescr->p_file);
        fwrite(&c,1, 1, trash);
    }
    // truncate("1.txt", 0);
    freopen(__pdescr->file_name, "wb", __pdescr->p_file);
    fseek(trash, 0, SEEK_SET);

    for (int a = 0;a <(move_from - strlen(__hash)-6);a++)
    {
        c = fgetc(trash);
        
        c =~ c + (C_XOR_1 << 2) + (C_XOR_2 & C_XOR_3);

        c   ^= ((__pdescr->hsum)); 

        fwrite(&c, 1,1, __pdescr->p_file);
    }
    remove(TEMP_TRASH);
    fclose(trash);
    
    return 0;
}

uint32_t encrypter(desc * p_files__, char *password) {
    fseek(p_files__->p_file, 0, SEEK_END);
    move_from = ftell(p_files__->p_file);
    fseek(p_files__->p_file, 0, SEEK_SET);

    int __err;
    char c;
    off_t key_len = strlen(password);
    int hand;
    fseek(p_files__->p_file, 0, SEEK_SET);

    for (int i = 0; (i < move_from); i++) {
        
        c = fgetc(p_files__->p_file);
        
        fseek(p_files__->p_file, i, SEEK_SET);
        
        c   ^= ((p_files__->hsum));

        c =~ c + (C_XOR_1 << 2) - (C_XOR_2 & C_XOR_3);
        fwrite(&c, sizeof(char), 1, p_files__->p_file);
    }

    __err = import_hsum(p_files__);

    h_error(&__err);

    fclose(p_files__->p_file);

    return 0;
}

unsigned int import_hsum(desc * __pdescr) {

    int result, hash;
    off_t lens;
    size_t written;

    char buffer[256] = FIND_HASH, *position;

    hash = __pdescr->hsum;

    position = buffer + strlen(FIND_HASH);

    result = sprintf(position, "%d", hash);

    if (result < 0) {
        perror("[import_hsum] ");
        fclose(__pdescr->p_file);
        fclose(__pdescr->p_trash_file);
        return FAILED;
    }

    lens = strlen(buffer);

    if (fprintf(__pdescr->p_file, "%s", buffer) < 0) {
        // perror("[STOP] fprintf");
        fclose(__pdescr->p_file);
        fclose(__pdescr->p_trash_file);
        return FAILED;
    }
    
    return 0;
}

unsigned int check_hsum(desc *__pdescr) {

    fseek(__pdescr->p_file, 0, SEEK_END);
    move_from = ftell(__pdescr->p_file);
    fseek(__pdescr->p_file, 0, SEEK_SET);

    // printf("len: [%i]\n\n", move_from);

    char __hash[255] = {0},c;
   
    int turned = 0, i = 0;
    
    char *buff = FIND_HASH; // Hotfx
    
    snprintf(__hash, sizeof(__hash), "%d", __pdescr->hsum);
    
    fseek(__pdescr->p_file, 0, SEEK_SET);
    
    static int r = 0;
    // printf("we are here");

    for (int a = 0, f = 0, z = 0; a < move_from; a++) 
    {

        c = fgetc(__pdescr->p_file);   
        // printf("%i\n", move_from);
        // if (copyhash == true)
        // printf("[%c]",c);
        
        // if (c == __hash[z] && f == 1) 
        // {
            
        //     printf("[%c]", c);
        //     z++;
        //     if (z == strlen(__hash)) {
        //         printf("we found hash");
        //         return 0;
        //     }
        //     // f++;
        // }
        // // else if (c != __hash[z]) {
        //     // printf("[%c]", c);
        //     f = 0;
        //     z = 0;
        //     r = 0;
            
        // }

        if (c == __hash[z] && f == 1) {
            // printf("%c", c);
            // exit(0);
            z++;
            if (z == strlen(__hash)) {
                // printf("GOOD\n");
                return 0;
            }
        }

        if (c == buff[r]) 
        {
            // printf("we are in: %c = %i\n", c, r);   
            if (r == strlen(buff)-1) {
                // printf("FOUND BEGIN\n");
                f = 1;
            }
            r+=1;
        }
        else {
            r = 0;
            // continue;
        }

    }

    return 1;
}
