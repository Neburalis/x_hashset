#include <stdio.h>

#include "../include/io_utils.h"
#include "../include/ansi.h"

const char* get_byte_color(const uint8_t byte) {
    if (byte == 0x00) return ANSI_FAINT ANSI_COLOR_WHITE;
    if (byte == 0xFF) return ANSI_BOLD  ANSI_COLOR_MAGENTA;
    if (byte <= 0x0F) return ANSI_COLOR_GREEN;
    if (byte <= 0x7F) return ANSI_COLOR_BRIGHT_YELLOW;
    return ANSI_COLOR_BRIGHT_RED;
}

bool is_printable(const uint8_t byte) {
    return byte >= 0x20 && byte <= 0x7E;
}

void memdump(void *ptr, const size_t bytes_count) {
    if (ptr == nullptr || bytes_count == 0)
        return;

    const uint8_t *p = static_cast<const uint8_t *>(ptr);
    const uintptr_t start_ptr = reinterpret_cast<uintptr_t>(ptr);
    const uintptr_t end_ptr   = reinterpret_cast<uintptr_t>(ptr) + bytes_count;

    const uintptr_t aligned_ptr = start_ptr & ~(0xFul);
    const size_t offset = aligned_ptr - start_ptr;

    printf(BOLD(YELLOW("               x0 x1 x2 x3  x4 x5 x6 x7  x8 x9 xA xB  xC xD xE xF\n")));
    for (uintptr_t addr = aligned_ptr; addr < end_ptr || (addr == aligned_ptr && offset > 0); addr += 16) {
        // Печатаем адрес строки
        printf(CYAN("0x%012lx "), addr);

        char ascii_repr[17] = {0};

        // Печатаем байты
        for (size_t i = 0; i < 16; i++) {
            const uintptr_t current_addr = addr + i;

            // Добавляем пробелы между группами по 4 байта
            if (i == 4 || i == 8 || i == 12) {
                printf(" ");
            }

            // Проверяем, находится ли текущий адрес в диапазоне
            if (current_addr < start_ptr || current_addr >= end_ptr) {
                printf(GRAY("-- ")); // Пустое место для байтов вне диапазона
                ascii_repr[i] = ' ';
            } else {
                // Вычисляем индекс в массиве данных
                const size_t data_index = current_addr - start_ptr;
                const uint8_t byte = p[data_index];
                printf("%s%02X " ANSI_COLOR_RESET , get_byte_color(byte), byte);

                ascii_repr[i] = is_printable(byte) ? byte : '.';
            }
        }

        printf(BLUE("%s\n"), ascii_repr);
    }
}