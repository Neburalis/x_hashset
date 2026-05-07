#include <stdint.h>
#include <stdio.h>

#include "io_utils.h"

int main() {
    uint8_t test_data[] = {
        0x00, 0x01, 0x0F, 0x10, 0x7F, 0x80, 0xFF, 0x42,
        'H', 'e', 'l', 'l', 'o', '!', 0x00, 0x00,
        0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, 0xBA, 0xBE,
        0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F
    };

    printf("Memory dump with colors:\n\n");
    memdump(test_data, sizeof(test_data));

    printf("\n\nMemory dump with unaligned pointer:\n\n");
    memdump(test_data + 3, 20);

    return 0;
}