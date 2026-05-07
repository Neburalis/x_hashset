#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <stdarg.h>

#include "../include/io_utils.h"

#define TYPED_CALLOC_(NMEMB, TYPE) \
    (TYPE *) calloc((NMEMB), sizeof(TYPE));

#define FREE_(ptr)     \
    free((ptr));       \
    (ptr) = nullptr;

int msleep(const long msec) {
    assert(msec >= 0 && "Time cannot be negative");

    if (msec < 0) {
        errno = EINVAL;
        return -1;
    }

    struct timespec ts = {};
    int res = 0;

    ts.tv_sec = msec / 1000;              // miliseconds in one second
    ts.tv_nsec = (msec % 1000) * 1000000; // nanoseconds io one second
    //                    ^----------------- remove int part seconds in ms

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

int nsleep(const size_t nsec) {
    struct timespec ts = {};
    int res = 0;

    ts.tv_sec  = static_cast<long>(nsec / 1000000000ULL); // miliseconds in one second
    ts.tv_nsec = static_cast<long>(nsec % 1000000000ULL); // nanoseconds io one second
    //                                   ^------- remove int part seconds in ms

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

void spinner(const char * const str, const uint32_t time, const uint32_t period) {
    assert(str != nullptr);

    if (str == nullptr) {
        errno = EINVAL;
        return;
    }

    const char pp[] = {'-', '\\', '|', '/'}; // spinner char
    for (uint32_t i = 0; i < static_cast<uint32_t>(time)/period; ++i) {
        printf("%s", str);
        printf("%c", pp[i % 4]);
        fflush(stdout);
        msleep(period);
        printf("\r");
    }
}

size_t lines_in_file(FILE * const fp) {
    assert(fp != nullptr && "You must pass FILE");

    if (!fp){
        errno = EBADF;
        return 0;
    }

    size_t lines = 1;
    int ch = 0;

    while(!feof(fp)) {
        ch = fgetc(fp);
        if(ch == '\n')
        {
            ++lines;
        }
    }

    if (fseek(fp, 0, SEEK_SET)) {
        errno = EBADF;
        return 0;
    }
    return lines;
}

int clear_stdin_buffer() {
    int c = 0;
    while ((c = getchar()) != '\n' && c != EOF)
        continue;
    return 1;
}

int is_stdin_buffer_clean() {
    int c = 0;
    while ((c = getchar()) != '\n' && c != EOF) {
        if (c == ' ' || c == '\t')
            continue;
        return 0;
    }
    return 1;
}

double safe_get_double(const char * const prompt) {
    assert(prompt != nullptr && "You must pass variable name");

    if (prompt == nullptr) {
        errno = EINVAL;
        return -1;
    }

    int scanf_status = 0;
    double var = NAN;
    for (;;) {
        printf("%s", prompt);
        scanf_status = scanf("%lg", &var);
        if (scanf_status == EOF) {
            fprintf(stderr, "Ввод завершен. Завершение программы.\n");
            return NAN;
        }
        if (scanf_status && !isinf(var) && isfinite(var) && is_stdin_buffer_clean())
            return var;
        clear_stdin_buffer();
        printf("😢 Вы ввели число, которое я не смог распознать.     \n"
               "Попробуйте ввести правильное число (например 12.34). \n"
               "Десятичный разделитель точка (.).                    \n");
    }
}

int is_user_want_continue(const char * const fmt, ...) {
    assert(fmt != nullptr && "You must pass message for user");

    if (fmt == nullptr) {
        errno = EINVAL;
        return -1;
    }

    int choice = 0;

    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    fflush(stdout);
    va_end(ap);

    do {
        choice = getchar();
    }
    while (choice != 'y' &&
           choice != 'Y' &&
           choice != 'n' &&
           choice != 'N' &&
           choice != '\n');

    if (choice != '\n') clear_stdin_buffer();

    if (choice == 'Y' || choice == 'y')
        return 1;
    return 0;
}

ssize_t file_byte_size(const char * const filename) {
    assert(filename != nullptr && "U must provide valid filename");

    if (filename == nullptr) {
        errno = EINVAL;
        return -2;
    }

    struct stat file_info = {};

    if (stat(filename, &file_info) == -1) {
        ERROR_MSG("Не удалось получить информацию о файле %s\n", filename);
        return -1;
    }

    return (ssize_t) file_info.st_size;
}

char * read_file_to_buf(const char * const filename, size_t * const buf_len) {
    if (filename == nullptr) {
        errno = EINVAL;
        return nullptr;
    }

    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        return nullptr; /* errno set by fopen */
    }

    /* Delegate to FILE* overload which handles regular and non-regular files */
    char *res = read_file_to_buf(fp, buf_len);

    /* Close the FILE we opened regardless of success or failure */
    const int saved_errno = errno;
    fclose(fp);
    errno = saved_errno;

    return res;
}

char * read_file_to_buf(FILE * file, size_t * const buf_len) {
    if (file == nullptr) {
        errno = EINVAL;
        return nullptr;
    }

    int fd = fileno(file);
    if (fd == -1) {
        errno = EBADF;
        return nullptr;
    }

    struct stat st = {};
    if (fstat(fd, &st) == 0 && S_ISREG(st.st_mode)) {
        /* Regular file: try to read remaining bytes from current position */
        off_t file_size = st.st_size;
        off_t cur = ftell(file);
        if (cur == -1) {
            /* cannot get current position, try to seek to start */
            if (fseek(file, 0, SEEK_SET) != 0) {
                return nullptr;
            }
            cur = 0;
        }

        off_t remain = file_size - cur;
        if (remain < 0) remain = 0;

        /* Check for overflow when adding terminating NUL */
        if ((unsigned long long)remain > (unsigned long long)SIZE_MAX - 1ULL) {
            errno = ENOMEM;
            return nullptr;
        }

        size_t alloc_size = (size_t)remain + 1;
        char *buf = TYPED_CALLOC_(alloc_size, char);
        if (!buf) return nullptr;

        if (remain > 0) {
            size_t nread = fread(buf, 1, (size_t)remain, file);
            if (nread != (size_t)remain) {
                int saved_errno = ferror(file) ? errno : EIO;
                FREE_(buf);
                errno = saved_errno;
                return nullptr;
            }
        }

        buf[remain] = '\0';
        if (buf_len)
            *buf_len = alloc_size;
        return buf;
    }

    /* Non-regular file (pipe, socket, tty, etc.) - read until EOF with growing buffer */
    constexpr size_t CHUNK = 4096;
    size_t cap = CHUNK;
    char *buf = TYPED_CALLOC_(cap, char);
    if (!buf) return nullptr;

    size_t total = 0;
    for (;;) {
        if (total == cap) {
            if (cap > SIZE_MAX - CHUNK) { /* overflow */
                FREE_(buf);
                errno = ENOMEM;
                return nullptr;
            }
            const size_t new_cap = cap + CHUNK;
            char *p = (char *) realloc(buf, new_cap);
            if (!p) {
                FREE_(buf);
                return nullptr;
            }
            buf = p;
            cap = new_cap;
        }

        const size_t to_read = cap - total;
        const size_t n = fread(buf + total, 1, to_read, file);
        if (n > 0) total += n;

        if (n < to_read) {
            if (feof(file)) break;
            if (ferror(file)) {
                int saved_errno = errno ? errno : EIO;
                FREE_(buf);
                errno = saved_errno;
                return nullptr;
            }
        }
    }

    /* ensure space for terminating NUL */
    if (total >= SIZE_MAX) {
        FREE_(buf);
        errno = ENOMEM;
        return nullptr;
    }
    char *p = (char *) realloc(buf, total + 1);
    if (!p) {
        FREE_(buf);
        return nullptr;
    }
    p[total] = '\0';
    if (buf_len)
        *buf_len = total + 1;
    return p;
}

size_t * read_file_to_size_t_buf(const char *filename, size_t *count) {
    // Проверки аргументов
    if (filename == nullptr || count == nullptr) {
        errno = EINVAL;
        return nullptr;
    }
    *count = 0;

    // Открываем файл в бинарном режиме
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        return nullptr; // errno установлен fopen()
    }

    // Получаем размер файла
    struct stat st = {};
    if (fstat(fileno(fp), &st) != 0) {
        fclose(fp);
        return nullptr; // errno установлен fstat()
    }

    long long file_size = st.st_size;

    // Проверка: файл должен быть кратен sizeof(size_t)
    if ((unsigned long long)file_size % sizeof(size_t) != 0) {
        fclose(fp);
        errno = EINVAL; // повреждённый или не тот формат
        return nullptr;
    }

    // Проверка: не превышает ли размер SSIZE_MAX (защита от переполнения при приведении)
    if (file_size > SSIZE_MAX) {
        fclose(fp);
        errno = EFBIG;
        return nullptr;
    }

    size_t total_bytes = (size_t)file_size;
    size_t n = total_bytes / sizeof(size_t);

    // Выделяем память (malloc, не calloc — данные бинарные, обнуление не нужно)
    size_t *buf = static_cast<size_t *>(malloc(total_bytes));
    if (!buf) {
        fclose(fp);
        return nullptr; // errno = ENOMEM (установлен malloc)
    }

    // Читаем весь файл за один раз
    size_t nread = fread(buf, 1, total_bytes, fp);
    if (nread != total_bytes) {
        int saved_errno = ferror(fp) ? errno : EIO;
        FREE_(buf);
        fclose(fp);
        errno = saved_errno;
        return nullptr;
    }

    fclose(fp);
    *count = n;
    return buf;
}

int create_folder_if_not_exists(const char *dir_path) {
    assert(dir_path != nullptr);

    if (dir_path == nullptr) {
        errno = EINVAL;
        return 0;
    }

    if (mkdir(dir_path, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
        if (errno == EEXIST) {
            struct stat st;
            if (lstat(dir_path, &st) == -1) {
                return -1;
            }

            if (!S_ISDIR(st.st_mode)) {
                errno = ENOTDIR;  // Существует, но не директория (возможно симлинк)
                return -1;
            }

            if (st.st_uid != getuid()) {
                errno = EPERM;  // Директория принадлежит другому пользователю
                return -1;
            }
            return 0; // Директория уже существует, является директорией и принадлежит правильному владельцу
        } else {
            perror("Error creating directory");
            return 1; // Indicate an error
        }
    } else {
        // Directory created successfully.
        return 0;
    }
}

#undef TYPED_CALLOC_
#undef FREE_