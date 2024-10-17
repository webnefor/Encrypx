
#include "config.h"

#include <sys/types.h>
#include <pthread.h>

static __uint128_t move_from = 0;

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

uint64_t set_size_of_file (desc * p_file ) {

    fseek(p_file->p_file, 0, SEEK_END);
    move_from = ftell(p_file->p_file);
    fseek(p_file->p_file, 0, SEEK_SET);

    return 0;
}

uint32_t f_decrypt(desc * _pdescr, char *password){

    __uint128_t i;
    size_t bytesRead;
    char * data_handler, __hash[BUFFER_SIZE], c;
    long int countXor __attribute__((aligned(sizeof(unsigned int))));
    unsigned int xorValue __attribute__((visibility("hidden")));

    snprintf(__hash, sizeof(__hash), "%d", _pdescr->hsum);

    off_t offbyte = move_from - (strlen(__hash) + strlen(FIND_HASH));
     
    fseek(_pdescr->p_file, 0, SEEK_SET);

    data_handler = calloc(BUFFER_SIZE, sizeof(char));
    i = 0;

    xorValue= (C_XOR_1 << 2) - (C_XOR_2 & C_XOR_3);
    countXor = SLOW;

    while (i < move_from)
    {
        bytesRead = fread(data_handler, sizeof(char), BUFFER_SIZE, _pdescr->p_file);

        fseek(_pdescr->p_file, -bytesRead, SEEK_CUR);

        i+= bytesRead;

        for (int li = 0; li < bytesRead; li++)
        {
            data_handler[li] = (data_handler[li] ^= ((xorValue << 2))) - countXor;
            data_handler[li] ^= _pdescr->hsum;
        }

      fwrite(data_handler, sizeof(char), bytesRead, _pdescr->p_file);
  }

  fclose(_pdescr->p_file);
  free(data_handler);
  truncate(_pdescr->file_name, offbyte);
  return 0;
}

uint32_t f_encrypt(desc * _pdescr, char *password)
{
    set_size_of_file(_pdescr);

    unsigned int xorValue __attribute__((visibility("hidden")));
    long int countXor __attribute__((aligned(sizeof(unsigned int))));
    uint_fast64_t i;
    uint_fast64_t li;
    int __err;
    char * data_handler;
    size_t bytesRead;

    data_handler = malloc(BUFFER_SIZE);
    i = 0;
    bytesRead = 0;

    xorValue = (C_XOR_1 << 2) - (C_XOR_2 & C_XOR_3);
    countXor = SLOW;

    while (i < move_from)
    {
        bytesRead = fread(data_handler, sizeof(char), BUFFER_SIZE, _pdescr->p_file);

        fseek(_pdescr->p_file, -bytesRead, SEEK_CUR);

        i+= bytesRead;

        for (int li = 0; li < bytesRead; li++)
        {
            data_handler[li] ^= _pdescr->hsum;
            data_handler[li] = (data_handler[li] ^= ((xorValue << 2))) + countXor;
        }

        fwrite(data_handler, sizeof(char), bytesRead, _pdescr->p_file);
    }

    free(data_handler);

    __err = importSum(_pdescr);

    h_error(&__err);

    fclose(_pdescr->p_file);

    return 0;
}

unsigned int importSum(desc * _pdescr) {

    fseek(_pdescr->p_file, 0, SEEK_END);

    int result, hash;

    char buffer[256] = FIND_HASH, *position;

    hash = _pdescr->hsum;

    position = buffer + strlen(FIND_HASH);

    result = sprintf(position, "%d", hash);

    if (result < 0) {
        perror("[importSum] ");
        fclose(_pdescr->p_file);
        fclose(_pdescr->p_trash_file);
        return FAILED;
    }

    if (fprintf(_pdescr->p_file, "%s", buffer) < 0) {
        fclose(_pdescr->p_file);
        fclose(_pdescr->p_trash_file);
        return FAILED;
    }
  
    return 0;
}

unsigned int validateHash(desc * _pdescr) {

    set_size_of_file(_pdescr);

    int bytes_to_read, result;
    char * buffer, *position, str_of_the_hash[255] = FIND_HASH;

    position = str_of_the_hash + strlen(FIND_HASH);

    result = sprintf(position, "%lld", _pdescr->hsum);

    h_error(&result);

    bytes_to_read = strlen(position) + strlen(FIND_HASH);
    buffer = (char *)calloc(bytes_to_read * 2, sizeof(char));


    if (fseek(_pdescr->p_file, -bytes_to_read, SEEK_END) != 0) {
        fclose(_pdescr->p_file);
        return 1;
    }

    // Allocate buffer to store the read data
    if (buffer == NULL) {
        fclose(_pdescr->p_file);
        return 1;
    }

    // Read the data from the file
    size_t read_bytes = fread(buffer, sizeof(char),bytes_to_read, _pdescr->p_file);

    if (read_bytes != bytes_to_read) {
        free(buffer);
        fclose(_pdescr->p_file);
        return 1;
    }

    if (strcmp(buffer, str_of_the_hash) != 0) {
      free(buffer);
      fclose(_pdescr->p_file);
      return 1;
    }

    // Null-terminate the buffer and print the content
     buffer[bytes_to_read] = '\0';

    // Clean up
    free(buffer);

    return 0;
}

void progress_bar( int i ) {

}
