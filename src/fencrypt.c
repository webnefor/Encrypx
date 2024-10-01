
#include "config.h"

static uint64_t move_from = 0;


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

uint32_t f_decrypt(desc * __pdescr, char *password) {

    char data_handler[BUFFER_SIZE],__hash[BUFFER_SIZE], c;
    int bytesRead, i;

    snprintf(__hash, sizeof(__hash), "%d", __pdescr->hsum);

    int totalBytesToRead = move_from - strlen(__hash) - strlen(FIND_HASH);
    
    while ((bytesRead = fread(data_handler, sizeof(char), BUFFER_SIZE, __pdescr->p_file)) > 0) {
        for (i = 0; i < bytesRead && i < move_from; i++) {
            data_handler[i] = ~data_handler[i] + (C_XOR_1 << 2) - (C_XOR_2 & C_XOR_3);
            data_handler[i] ^= __pdescr->hsum;
        }
        fseek(__pdescr->p_file, -bytesRead, SEEK_CUR);
        fwrite(data_handler, sizeof(char), bytesRead, __pdescr->p_file);
    }
    truncate(__pdescr->file_name,totalBytesToRead);

    return 0;
}

uint32_t f_encrypt(desc * p_files__, char *password) {

    // fseek(p_files__->p_file, 0, SEEK_END);
    // move_from = ftell(p_files__->p_file);
    // fseek(p_files__->p_file, 0, SEEK_SET);

    set_size_of_file(p_files__);

    int __err;
    char c;
    // fseek(p_files__->p_file, 0, SEEK_SET);
    char buffer[BUFFER_SIZE];
    size_t bytesRead;
    int i;

    // Перемещение указателя файла в начало
    fseek(p_files__->p_file, 0, SEEK_SET);

    // Чтение файла блоками
    while ((bytesRead = fread(buffer, sizeof(char), BUFFER_SIZE, p_files__->p_file)) > 0) {

        for (i = 0; i < bytesRead && i < move_from; i++) {
            buffer[i] ^= p_files__->hsum;
            buffer[i] = ~buffer[i] + (C_XOR_1 << 2) - (C_XOR_2 & C_XOR_3);
        }
        // Запись обратно в файл
        fseek(p_files__->p_file, -bytesRead, SEEK_CUR);
        fwrite(buffer, sizeof(char), bytesRead, p_files__->p_file);

        move_from -= bytesRead;  // Уменьшаем количество оставшихся символов для обработки

        if (move_from <= 0) 
            break;  // Выход из цикла, если обработаны все символы
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
        fclose(__pdescr->p_file);
        fclose(__pdescr->p_trash_file);
        return FAILED;
    }
    
    return 0;
}

unsigned int check_hsum(desc *__pdescr) {

    set_size_of_file(__pdescr);

    char __hash[255] = {0},c;
    char *buff = FIND_HASH; // Hotfx
    size_t bytesRead;
    char buffer[BUFFER_SIZE];
    snprintf(__hash, sizeof(__hash), "%d", __pdescr->hsum);
    
    int r = 0, z = 0, f = 0, p = 0;

    int i = 0;    

    while ((bytesRead = fread(buffer, sizeof(char), BUFFER_SIZE, __pdescr->p_file)) > 0) {
        
        fseek(__pdescr->p_file, bytesRead, SEEK_CUR);

        for (;i < bytesRead;i++) {
            if (buffer[i] == __hash[z] && f == 1)
            {
                z++;
                if (z == strlen(__hash))
                {
                    fseek(__pdescr->p_file, 0, SEEK_SET);
                    return 0;
                }
            }

            if (buffer[i] == buff[r] && f != 1) 
            {
                if (r == strlen(buff)-1) {

                    f = 1;
                }
                r += 1;
            }
            else 
            {
                p=0;
            }
        }
        i = 0;
    }
    return 1;
}

uint64_t set_size_of_file (desc * p_file ) {

    fseek(p_file->p_file, 0, SEEK_END);
    move_from = ftell(p_file->p_file);
    fseek(p_file->p_file, 0, SEEK_SET);

    return 0;
}


void progress_bar( int i ) {

    // in developing

}
