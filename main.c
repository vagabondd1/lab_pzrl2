#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <regex.h>
#include "lab2.h"

int main(int argc, char *argv[argc])
{
    if (argc < 3)
    {
        printf("Недостаточное кол-во аргументов.\n");
        return 0;
    }
    char *str = argv[1];
    if (!strchr(str, '.') || strlen(str) < 5)
    {
        printf("Передано что-то отличное от файла.\n");
        return 0;
    }
    size_t index_dot = strchr(str, '.') - str;

    size_t ext_len = strlen(str) - index_dot - 1;
    char ext[ext_len + 1];
    memcpy(ext, str + index_dot + 1, ext_len);
    ext[ext_len] = '\0';

    if (strcmp(ext, "txt") != 0)
    {
        printf("Расширение файла содержит ошибку.\n");
        return 0;
    }

    char *pattern = argv[2];
    bool flag;
    if (strchr(pattern, 'd') == 0 && strchr(pattern, '^') == 0 && strchr(pattern, '$') == 0)
    {
        flag = check_replace(pattern, str);
    }
    else if (strchr(pattern, '^'))
    {
        flag = check_prefix(pattern,str);
    }
    else if (strchr(pattern, '$'))
    {
        flag = check_suffix(pattern, str);
    }
    else
    {
        flag = check_remove(pattern, str);
    }
    return flag ? 0 : 1;
}