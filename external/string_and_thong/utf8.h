#ifndef UTF8_H
#define UTF8_H

#include <stddef.h>
#include <stdint.h>

// Проверит, является ли байт началом UTF8 символа
// Байт является началом символа, если биты 7-6 НЕ равны 10 (10xxxxxx)
bool is_utf8_start_byte(unsigned char c);

// Подсчет количества UTF-8 символов в строке (strlen для utf8 строки вернет число байтов)
size_t utf8_strlen(const char *str);

// Получение длины UTF-8 символа в байтах
uint8_t utf8_char_length(unsigned char first_byte);

// Получение n-го UTF-8 символа в строке
const char* utf8_char_at(const char *str, size_t index);

// Извлечение кодовой точки UTF-8
int utf8_char_to_codepoint(const char *utf8_char, int length);

// Получение следующего UTF-8 символа
// Возвращает указатель на следующий символ или NULL, если достигнут конец строки
// example:
/*
const char *pos = text;
int length = 0;
size_t index = 0;
while ((pos = utf8_next_char(pos, &length)) != NULL) {
    int codepoint = utf8_char_to_codepoint(pos, length);
    const char *c = utf8_char_at();
    printf("Символ %zu (offset = %): U+%04X (%.*s)\n", index, codepoint, len, c);
    pos += length;
    index++;
}
*/
const char* utf8_next_char(const char *current_pos, int *char_length);

#endif //UTF8_H