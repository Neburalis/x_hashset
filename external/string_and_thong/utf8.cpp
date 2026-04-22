#include "utf8.h"

// Проверит, является ли байт началом UTF8 символа
// Байт является началом символа, если биты 7-6 НЕ равны 10 (10xxxxxx)
bool is_utf8_start_byte(unsigned char c) {
    return (c & 0b11000000) != 0b10000000;
}

// Подсчет количества UTF-8 символов в строке (strlen для utf8 строки вернет число байтов)
size_t utf8_strlen(const char *str) {
    size_t count = 0;
    for (const char *p = str; *p != '\0'; p++) {
        if (is_utf8_start_byte(*p)) {
            count++;
        }
    }
    return count;
}

// Получение длины UTF-8 символа в байтах
uint8_t utf8_char_length(unsigned char first_byte) {
    // 1 байт: 0xxxxxxx (ASCII)
    if ((first_byte & 0b10000000) == 0)
        return 1;        // 0xxxxxxx

    // 2 байта: 110xxxxx
    if ((first_byte & 0b11100000) == 0b11000000)
        return 2;        // 110xxxxx

    // 3 байта: 1110xxxx
    if ((first_byte & 0b11110000) == 0b11100000)
        return 3;        // 1110xxxx

    // 4 байта: 11110xxx
    if ((first_byte & 0b11111000) == 0b11110000)
        return 4;        // 11110xxx

    return 1;  // Невалидный байт - считаем как 1 байт
}

// Получение n-го UTF-8 символа в строке
const char* utf8_char_at(const char *str, size_t index) {
    size_t current = 0;
    for (const char *p = str; *p != '\0'; p++) {
        if (is_utf8_start_byte(*p)) {
            if (current == index) return p;
            current++;
        }
    }
    return NULL;
}

// Дополнительная функция: извлечение кодовой точки UTF-8
int utf8_char_to_codepoint(const char *utf8_char, int length) {
    if (length == 1) {
        return utf8_char[0];  // ASCII
    }

    int codepoint = 0;

    switch (length) {
        case 2: // 110xxxxx 10xxxxxx
            codepoint = (utf8_char[0] & 0b00011111) << 6;
            codepoint |= (utf8_char[1] & 0b00111111);
            break;

        case 3: // 1110xxxx 10xxxxxx 10xxxxxx
            codepoint = (utf8_char[0] & 0b00001111) << 12;
            codepoint |= (utf8_char[1] & 0b00111111) << 6;
            codepoint |= (utf8_char[2] & 0b00111111);
            break;

        case 4: // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            codepoint = (utf8_char[0] & 0b00000111) << 18;
            codepoint |= (utf8_char[1] & 0b00111111) << 12;
            codepoint |= (utf8_char[2] & 0b00111111) << 6;
            codepoint |= (utf8_char[3] & 0b00111111);
            break;
    }

    return codepoint;
}

// Получение следующего UTF-8 символа
// Возвращает указатель на следующий символ или NULL, если достигнут конец строки
const char* utf8_next_char(const char *current_pos, int *char_length) {
    if (!current_pos || *current_pos == '\0') {
        if (char_length) *char_length = 0;
        return NULL;
    }

    int length = utf8_char_length(*(unsigned char*)current_pos);

    // Проверяем, что у нас достаточно байт до конца строки
    const char *next_pos = current_pos + length;
    for (int i = 1; i < length; i++) {
        if (current_pos[i] == '\0') {
            // Неполный символ, возвращаем то что есть
            if (char_length) *char_length = i;
            return current_pos;
        }
        // Проверяем, что продолжaющие байты имеют правильную структуру 10xxxxxx
        if ((current_pos[i] & 0b11000000) != 0b10000000) {
            if (char_length) *char_length = i;
            return current_pos;
        }
    }

    if (char_length) *char_length = length;
    return next_pos;
}