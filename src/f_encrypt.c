
#include "config.h"

__uint128_t move_from = 0;

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

uint32_t f_decrypt(desc * dateList, char *password){

    long bytesRead;
    uint64_t progress = 0;

    char * data_handler, __hash[BUFFER_SIZE], c;

    int countXor __attribute__((aligned(sizeof(unsigned int))));

    int xorValue __attribute__((visibility("hidden")));

    snprintf(__hash, sizeof(__hash), "%d", dateList->hsum);

    off_t offbyte = move_from - (strlen(__hash) + strlen(FIND_HASH));

    data_handler = malloc(BUFFER_SIZE);

    xorValue        = (C_XOR_1 << 2) - (C_XOR_2 & C_XOR_3);

    countXor = SLOW;

    fseek(dateList->p_file, 0, SEEK_SET);

    do {
        bytesRead = fread(data_handler, sizeof(char),
                          BUFFER_SIZE, dateList->p_file);

        progress += bytesRead;

        fseek(dateList->p_file, -bytesRead , SEEK_CUR);

        for (int i = 0, out = bytesRead; i <= bytesRead; i++, out--)
        {
            data_handler[i]         ^= dateList->hsum;

            data_handler[out + i]   += (dateList->hsum & xorValue);

            data_handler[i]         -= (C_XOR_3 & (C_XOR_4+i));
            data_handler[i]         ^= (dateList->hsum & i);
            data_handler[out]       -= (C_XOR_3);
        }

        fwrite(data_handler, sizeof(char), bytesRead, dateList->p_file);

        fseek(dateList->p_file, 0 , SEEK_CUR);
        progress_bar(progress, move_from);

    } while (bytesRead > 0);

    fclose(dateList->p_file);
    free(data_handler);
    truncate(dateList->file_name, offbyte);

    return OK;

}

uint32_t f_encrypt(desc * dateList, char *password)
{
    set_size_of_file(dateList);
    // printf("[[[%lld]]]\n",move_from );
    int xorValue __attribute__((visibility("hidden")));
    int countXor __attribute__((aligned(sizeof(unsigned int))));
    int __err;

    char * data_handler;

    long bytesRead;
    uint64_t progress = 0;

    data_handler = malloc(BUFFER_SIZE);

    bytesRead = 0;

    xorValue = (C_XOR_1 << 2) - (C_XOR_2 & C_XOR_3);
    countXor = SLOW;

    fseek(dateList->p_file, 0 , SEEK_SET);

    do {
        bytesRead = fread(data_handler, sizeof(char),
                          BUFFER_SIZE, dateList->p_file);

        fseek(dateList->p_file, -bytesRead , SEEK_CUR);
        progress += bytesRead;

        for (int i = bytesRead, out = 0; i >= 0 ; i--, out++)
        {
            data_handler[i]         ^= (dateList->hsum & i);
            data_handler[i]         += (C_XOR_3 & (C_XOR_4+i));
            data_handler[out]       += (C_XOR_3);

            data_handler[out + i]   -= (dateList->hsum & xorValue);

            data_handler[i]         ^= dateList->hsum;
        };

        fwrite(data_handler, sizeof(char), bytesRead, dateList->p_file);

        fseek(dateList->p_file, 0 , SEEK_CUR);
        progress_bar(progress, move_from);

    } while (bytesRead > 0);

    free(data_handler);

    __err = importSum(dateList);

    h_error(&__err);

    fclose(dateList->p_file);

    return OK;
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

void progress_bar(uint64_t progress, uint64_t total)
{
    int barWidth    = 70; // width of the progress bar
    float ratio     = (float)progress / total;
    int pos         = barWidth * ratio;

    printf("[");

    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) {
            printf(">");
        } else {
            printf(" ");
        }
    }

    printf("] %.1f%%\r", ratio * 100);
    fflush(stdout); // flush te output buffer
}
