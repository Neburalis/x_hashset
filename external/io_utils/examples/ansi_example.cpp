#include <stdio.h>
#include "ansi.h"

int main() {
    printf("\n" BOLD("=== ANSI Color Library Demo ===") "\n\n");

    // 1. Основные цвета текста
    printf(BOLD("1. Основные цвета текста:\n"));
    printf(BLACK("Black ") RED("Red ") GREEN("Green ") YELLOW("Yellow ") 
           BLUE("Blue ") MAGENTA("Magenta ") CYAN("Cyan ") WHITE("White\n\n"));

    // 2. Яркие цвета текста
    printf(BOLD("2. Яркие цвета (High-Intensity):\n"));
    printf(BRIGHT_BLACK("Bright Black ") BRIGHT_RED("Bright Red ") 
           BRIGHT_GREEN("Bright Green ") BRIGHT_YELLOW("Bright Yellow ") 
           BRIGHT_BLUE("Bright Blue ") BRIGHT_MAGENTA("Bright Magenta ") 
           BRIGHT_CYAN("Bright Cyan ") BRIGHT_WHITE("Bright White\n\n"));

    // 3. Цвета фона
    printf(BOLD("3. Цвета фона:\n"));
    printf(ON_BLACK("  Black  ") ON_RED("  Red  ") ON_GREEN("  Green  ") 
           ON_YELLOW("  Yellow  ") ON_BLUE("  Blue  ") ON_MAGENTA("  Magenta  ") 
           ON_CYAN("  Cyan  ") ON_WHITE("  White  ") "\n\n");

    // 4. Яркие цвета фона
    printf(BOLD("4. Яркие цвета фона:\n"));
    printf(ON_BRIGHT_BLACK("  B.Black  ") ON_BRIGHT_RED("  B.Red  ") 
           ON_BRIGHT_GREEN("  B.Green  ") ON_BRIGHT_YELLOW("  B.Yellow  ") 
           ON_BRIGHT_BLUE("  B.Blue  ") ON_BRIGHT_MAGENTA("  B.Magenta  ") 
           ON_BRIGHT_CYAN("  B.Cyan  ") ON_BRIGHT_WHITE("  B.White  ") "\n\n");

    // 5. Стили текста
    printf(BOLD("5. Стили текста:\n"));
    printf(BOLD("Bold text") " | " FAINT("Faint text") " | " 
           ITALIC("Italic text") " | " UNDERLINE("Underlined text") " | " INVERSE("Inverse colors") "\n");
    printf(BLINK("Blinking text") " | "
           STRIKETHROUGH("Strikethrough text") " (Не работает на macOS)" "\n\n");

    // 6. Комбинации цветов и стилей
    printf(BOLD("6. Комбинации:\n"));
    printf(ANSI_BOLD ANSI_COLOR_RED "Bold Red" ANSI_COLOR_RESET "\n");
    printf(ANSI_UNDERLINE ANSI_COLOR_BLUE "Underlined Blue" ANSI_COLOR_RESET "\n");
    printf(ANSI_ITALIC ANSI_COLOR_GREEN ANSI_BG_YELLOW "Italic Green on Yellow" ANSI_COLOR_RESET "\n");
    printf(ANSI_BOLD ANSI_COLOR_BRIGHT_CYAN ANSI_BG_BLACK "Bold Bright Cyan on Black" ANSI_COLOR_RESET "\n\n");

    // 7. 256-цветная палитра - образцы
    printf(BOLD("7. 256-цветная палитра (примеры):\n"));
    printf("Стандартные цвета (0-15): ");
    for (int i = 0; i < 16; i++) {
        printf(ANSI_FG_256(%d) "██" ANSI_COLOR_RESET, i);
    }
    printf("\n\n");

    // RGB куб - несколько примеров
    printf("RGB куб (216 цветов, примеры):\n");
    for (int r = 0; r < 6; r++) {
        for (int g = 0; g < 6; g++) {
            for (int b = 0; b < 6; b++) {
                int n = 16 + 36*r + 6*g + b;
                printf(ANSI_FG_256(%d) "█" ANSI_COLOR_RESET, n);
            }
        }
        printf("\n");
    }
    printf("\n");

    // Градации серого
    printf("Градации серого (24 уровня):\n");
    for (int i = 0; i < 24; i++) {
        int n = 232 + i;
        printf(ANSI_FG_256(%d) "██" ANSI_COLOR_RESET, n);
    }
    printf("\n\n");

    return 0;
}