#ifndef ANSI_H
#define ANSI_H

// --- Основные цвета  ---
#define ANSI_COLOR_BLACK   "\x1b[30m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_WHITE   "\x1b[37m"
#define ANSI_COLOR_DEFAULT "\x1b[39m"

// --- High-Intensity цвета ---
#define ANSI_COLOR_BRIGHT_BLACK  "\x1b[90m"  // Серый
#define ANSI_COLOR_BRIGHT_RED    "\x1b[91m"
#define ANSI_COLOR_BRIGHT_GREEN  "\x1b[92m"
#define ANSI_COLOR_BRIGHT_YELLOW "\x1b[93m"
#define ANSI_COLOR_BRIGHT_BLUE   "\x1b[94m"
#define ANSI_COLOR_BRIGHT_MAGENTA "\x1b[95m"
#define ANSI_COLOR_BRIGHT_CYAN   "\x1b[96m"
#define ANSI_COLOR_BRIGHT_WHITE  "\x1b[97m"

// --- Цвета фона ---
#define ANSI_BG_BLACK   "\x1b[40m"
#define ANSI_BG_RED     "\x1b[41m"
#define ANSI_BG_GREEN   "\x1b[42m"
#define ANSI_BG_YELLOW  "\x1b[43m"
#define ANSI_BG_BLUE    "\x1b[44m"
#define ANSI_BG_MAGENTA "\x1b[45m"
#define ANSI_BG_CYAN    "\x1b[46m"
#define ANSI_BG_WHITE   "\x1b[47m"
#define ANSI_BG_DEFAULT "\x1b[49m"

// --- High-Intensity цвета фона ---
#define ANSI_BG_BRIGHT_BLACK    "\x1b[100m"
#define ANSI_BG_BRIGHT_RED      "\x1b[101m"
#define ANSI_BG_BRIGHT_GREEN    "\x1b[102m"
#define ANSI_BG_BRIGHT_YELLOW   "\x1b[103m"
#define ANSI_BG_BRIGHT_BLUE     "\x1b[104m"
#define ANSI_BG_BRIGHT_MAGENTA  "\x1b[105m"
#define ANSI_BG_BRIGHT_CYAN     "\x1b[106m"
#define ANSI_BG_BRIGHT_WHITE    "\x1b[107m"

// --- Стили текста ---
#define ANSI_BOLD                   "\x1b[1m"
#define ANSI_FAINT                  "\x1b[2m"
#define ANSI_ITALIC                 "\x1b[3m"
#define ANSI_UNDERLINE              "\x1b[4m"
#define ANSI_BLINKING               "\x1b[5m"
#define ANSI_INVERSE                "\x1b[7m"
#define ANSI_HIDDEN                 "\x1b[8m"
#define ANSI_STRIKETHROUGH          "\x1b[9m"
#define ANSI_BOLD_RESET             "\x1b[22m"
#define ANSI_FAINT_RESET            "\x1b[22m"
#define ANSI_ITALIC_RESET           "\x1b[23m"
#define ANSI_UNDERLINE_RESET        "\x1b[24m"
#define ANSI_BLINKING_RESET         "\x1b[25m"
#define ANSI_INVERSE_RESET          "\x1b[27m"
#define ANSI_HIDDEN_RESET           "\x1b[28m"
#define ANSI_STRIKETHROUGH_RESET    "\x1b[29m"

#define BOLD(string)            ANSI_BOLD          string ANSI_BOLD_RESET
#define FAINT(string)           ANSI_FAINT         string ANSI_FAINT_RESET
#define ITALIC(string)          ANSI_ITALIC        string ANSI_ITALIC_RESET
#define UNDERLINE(string)       ANSI_UNDERLINE     string ANSI_UNDERLINE_RESET
#define BLINK(string)           ANSI_BLINKING      string ANSI_BLINKING_RESET
#define INVERSE(string)         ANSI_INVERSE       string ANSI_INVERSE_RESET
#define HIDDEN(string)          ANSI_HIDDEN        string ANSI_HIDDEN_RESET
#define STRIKETHROUGH(string)   ANSI_STRIKETHROUGH string ANSI_STRIKETHROUGH_RESET

// --- Сброс всех атрибутов ---
#define ANSI_COLOR_RESET   "\x1b[0m"

// --- Цветной вывод текста ---
#define BLACK(string)      ANSI_COLOR_BLACK   string ANSI_COLOR_RESET
#define RED(string)        ANSI_COLOR_RED     string ANSI_COLOR_RESET
#define GREEN(string)      ANSI_COLOR_GREEN   string ANSI_COLOR_RESET
#define YELLOW(string)     ANSI_COLOR_YELLOW  string ANSI_COLOR_RESET
#define BLUE(string)       ANSI_COLOR_BLUE    string ANSI_COLOR_RESET
#define MAGENTA(string)    ANSI_COLOR_MAGENTA string ANSI_COLOR_RESET
#define CYAN(string)       ANSI_COLOR_CYAN    string ANSI_COLOR_RESET
#define WHITE(string)      ANSI_COLOR_WHITE   string ANSI_COLOR_RESET

#define GRAY(string)           ANSI_COLOR_BRIGHT_BLACK   string ANSI_COLOR_RESET

#define BRIGHT_BLACK(string)   ANSI_COLOR_BRIGHT_BLACK   string ANSI_COLOR_RESET
#define BRIGHT_RED(string)     ANSI_COLOR_BRIGHT_RED     string ANSI_COLOR_RESET
#define BRIGHT_GREEN(string)   ANSI_COLOR_BRIGHT_GREEN   string ANSI_COLOR_RESET
#define BRIGHT_YELLOW(string)  ANSI_COLOR_BRIGHT_YELLOW  string ANSI_COLOR_RESET
#define BRIGHT_BLUE(string)    ANSI_COLOR_BRIGHT_BLUE    string ANSI_COLOR_RESET
#define BRIGHT_MAGENTA(string) ANSI_COLOR_BRIGHT_MAGENTA string ANSI_COLOR_RESET
#define BRIGHT_CYAN(string)    ANSI_COLOR_BRIGHT_CYAN    string ANSI_COLOR_RESET
#define BRIGHT_WHITE(string)   ANSI_COLOR_BRIGHT_WHITE   string ANSI_COLOR_RESET

// --- Макросы для фона ---
#define ON_BLACK(string)   ANSI_BG_BLACK   string ANSI_COLOR_RESET
#define ON_RED(string)     ANSI_BG_RED     string ANSI_COLOR_RESET
#define ON_GREEN(string)   ANSI_BG_GREEN   string ANSI_COLOR_RESET
#define ON_YELLOW(string)  ANSI_BG_YELLOW  string ANSI_COLOR_RESET
#define ON_BLUE(string)    ANSI_BG_BLUE    string ANSI_COLOR_RESET
#define ON_MAGENTA(string) ANSI_BG_MAGENTA string ANSI_COLOR_RESET
#define ON_CYAN(string)    ANSI_BG_CYAN    string ANSI_COLOR_RESET
#define ON_WHITE(string)   ANSI_BG_WHITE   string ANSI_COLOR_RESET

#define ON_BRIGHT_BLACK(string)     ANSI_BG_BRIGHT_BLACK    string ANSI_COLOR_RESET
#define ON_BRIGHT_RED(string)       ANSI_BG_BRIGHT_RED      string ANSI_COLOR_RESET
#define ON_BRIGHT_GREEN(string)     ANSI_BG_BRIGHT_GREEN    string ANSI_COLOR_RESET
#define ON_BRIGHT_YELLOW(string)    ANSI_BG_BRIGHT_YELLOW   string ANSI_COLOR_RESET
#define ON_BRIGHT_BLUE(string)      ANSI_BG_BRIGHT_BLUE     string ANSI_COLOR_RESET
#define ON_BRIGHT_MAGENTA(string)   ANSI_BG_BRIGHT_MAGENTA  string ANSI_COLOR_RESET
#define ON_BRIGHT_CYAN(string)      ANSI_BG_BRIGHT_CYAN     string ANSI_COLOR_RESET
#define ON_BRIGHT_WHITE(string)     ANSI_BG_BRIGHT_WHITE    string ANSI_COLOR_RESET

// --- 256-цветная палитра ---
//
// Формат: \x1b[38;5;<N>m — цвет текста
//         \x1b[48;5;<N>m — цвет фона
//
// Генератор для RGB куба (цвета 16-231)
// N = 16 + 36*r + 6*g + b, где r,g,b in [0,5]
#define ANSI_FG_256(n) "\x1b[38;5;" #n "m"
#define ANSI_BG_256(n) "\x1b[48;5;" #n "m"

// RGB куб (r,g,b 0..5)
#define ANSI_FG_CUBE(r, g, b) "\x1b[38;5;" XSTR(16 + (r)*36 + (g)*6 + (b)) "m"
#define ANSI_BG_CUBE(r, g, b) "\x1b[48;5;" XSTR(16 + (r)*36 + (g)*6 + (b)) "m"

// Градации серого (232-255) — 24 уровня
#define ANSI_FG_GRAY(level) "\x1b[38;5;" XSTR(232 + (level)) "m"  // level 0..23
#define ANSI_BG_GRAY(level) "\x1b[48;5;" XSTR(232 + (level)) "m"

#define XSTR(s) STR(s)
#define STR(s) #s

#define FG256(color, string) ANSI_FG_256(color) string ANSI_COLOR_RESET
#define BG256(color, string) ANSI_BG_256(color) string ANSI_COLOR_RESET

#define FG_CUBE(r, g, b, string) ANSI_FG_CUBE(r, g, b) string ANSI_COLOR_RESET
#define BG_CUBE(r, g, b, string) ANSI_BG_CUBE(r, g, b) string ANSI_COLOR_RESET

#define FG_GRAY(level, string) ANSI_FG_GRAY(level) string ANSI_COLOR_RESET
#define BG_GRAY(level, string) ANSI_BG_GRAY(level) string ANSI_COLOR_RESET

#endif // ANSI_H