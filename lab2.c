#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <regex.h>
#include <stdlib.h>

void rewrite_file(char **mass_of_lines, const char *filename, int count)
{
    FILE *file = fopen(filename, "w");
    for (int i = 0; i < count; i++)
    {
        fprintf(file, "%s", mass_of_lines[i]);
        free(mass_of_lines[i]);
    }
    free(mass_of_lines);
    fclose(file);
}

size_t count_lines_in_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    size_t line_count = 0;
    char ch;
    char prev_ch = '\0';
    while ((ch = fgetc(file)) != EOF)
    {
        if (ch == '\n')
        {
            ++line_count;
        }
        prev_ch = ch;
    }
    if (prev_ch != '\0' && prev_ch != '\n')
    {
        ++line_count;
    }
    fclose(file);
    return line_count;
}

bool check_replace(char *pattern, char *name_file)
{
    if (strncmp(pattern, "s/", 2) != 0)
    {
        printf("Строка не начинается с 's/'.\n");
        return 0;
    }

    const char *scnd_slash = strchr(pattern + 2, '/');
    if (!scnd_slash)
    {
        printf("Не найдена второй '/'.\n");
        return 0;
    }

    const char *thrd_slash = strchr(scnd_slash + 1, '/');
    if (!thrd_slash)
    {
        printf("Не найден третий '/'.\n");
        return 0;
    }
    if (pattern[thrd_slash - pattern + 1] != '\0')
    {
        printf("%c", pattern[thrd_slash - pattern + 1]);
        printf("После третьего '/' в строке есть что-то еще.\n");
        return 0;
    }
    const char *frst_slash = strchr(pattern, '/');

    size_t regex_len = scnd_slash - (frst_slash + 1);
    char rgx[regex_len + 1];
    strncpy(rgx, frst_slash + 1, regex_len);
    rgx[regex_len] = '\0';

    size_t replace_len = thrd_slash - (scnd_slash + 1);
    char replacement[replace_len + 1];
    strncpy(replacement, scnd_slash + 1, replace_len);
    replacement[replace_len] = '\0';

    regex_t regex;
    regmatch_t pmatch[1];

    int reti = regcomp(&regex, rgx, REG_EXTENDED);
    if (reti)
    {
        printf("Ошибка компиляции регулярного выражения: %s\n", rgx);
        return 0;
    }

    FILE *file = fopen(name_file, "r");
    if (!file)
    {
        printf("Ошибка открытия файла.\n");
        regfree(&regex);
        return 0;
    }
    int count = count_lines_in_file(name_file);
    if (count == 0)
    {
        printf("Файл пустой.\n");
        return 0;
    }
    char **mass_of_lines = (char **)calloc(count, sizeof(char *));
    char *curr_line = NULL;
    size_t len_line = 0;
    int flag1, flag2 = 0;
    unsigned int i = 0;

    while ((flag1 = getline(&curr_line, &len_line, file)) != -1)
    {
        flag2 = 1;
        size_t new_len = len_line;
        char *current_pos = curr_line;

        while (regexec(&regex, current_pos, 1, pmatch, 0) == 0)
        {
            new_len += strlen(replacement) - (pmatch[0].rm_eo - pmatch[0].rm_so);

            if (pmatch[0].rm_so == pmatch[0].rm_eo)
            {
                current_pos++;
                if (*current_pos == '\0')
                {
                    break;
                }
            }
            else
            {
                current_pos += pmatch[0].rm_eo;
            }
        }
        if (!flag2)
        {
            printf("Нет совпадений.\n");
            free(curr_line);
            fclose(file);
            regfree(&regex);
            return 0;
        }
        char *result = (char *)malloc(new_len + 1);
        if (!result)
        {
            printf("Не удалось выделить память.\n");
            free(curr_line);
            fclose(file);
            regfree(&regex);
            return 0;
        }

        current_pos = curr_line;
        char *result_pos = result;
        while (regexec(&regex, current_pos, 1, pmatch, 0) == 0)
        {
            int before_match_len = pmatch[0].rm_so;
            strncpy(result_pos, current_pos, before_match_len);
            result_pos += before_match_len;

            strcpy(result_pos, replacement);
            result_pos += strlen(replacement);

            if (pmatch[0].rm_so == pmatch[0].rm_eo)
            {
                current_pos++;
                if (*current_pos == '\0')
                {
                    break;
                }
            }
            else
            {
                current_pos += pmatch[0].rm_eo;
            }
        }

        strcpy(result_pos, current_pos);
        mass_of_lines[i++] = result;
    }

    free(curr_line);
    fclose(file);
    regfree(&regex);
    rewrite_file(mass_of_lines, name_file, count);
    if (flag1 == -1 && flag2 != 1)
    {
        printf("Файл пуст или ошибка чтения.\n");
        return 0;
    }
    return 1;
}

bool check_remove(char *pattern, char *name_file)
{
    if (strncmp(pattern, "/", 1) != 0)
    {
        printf("Строка не начинается с '/'.\n");
        return 0;
    }

    const char *scnd_slash = strchr(pattern + 2, '/');
    if (!scnd_slash)
    {
        printf("Не найдена второй '/'.\n");
        return 0;
    }

    const char *symb_end = strchr(scnd_slash + 1, 'd');
    if (!symb_end)
    {
        printf("Не найден символ 'd'.\n");
    }
    if (pattern[symb_end - pattern + 1] != '\0')
    {
        printf("После символа 'd' в строке есть что-то еще.\n");
        return 0;
    }
    size_t regex_len = scnd_slash - pattern - 1;
    char rgx[regex_len + 1];
    strncpy(rgx, pattern + 1, regex_len);
    rgx[regex_len] = '\0';

    regex_t regex;
    regmatch_t pmatch[1];

    int reti = regcomp(&regex, rgx, REG_EXTENDED);
    if (reti)
    {
        printf("Ошибка компиляции регулярного выражения: %s\n", rgx);
        return 0;
    }

    FILE *file = fopen(name_file, "r");
    if (!file)
    {
        printf("Ошибка открытия файла.\n");
        regfree(&regex);
        return 0;
    }
    int count = count_lines_in_file(name_file);
    if (count == 0)
    {
        printf("Файл пустой.\n");
        return 0;
    }
    char **mass_of_lines = (char **)calloc(count, sizeof(char *));
    char *curr_line = NULL;
    size_t len_line = 0;
    int flag1, flag2;
    unsigned int i = 0;

    while ((flag1 = getline(&curr_line, &len_line, file)) != -1)
    {
        flag2 = 1;
        size_t new_len = len_line;
        char *current_pos = curr_line;

        while (regexec(&regex, current_pos, 1, pmatch, 0) == 0)
        {
            new_len -= (pmatch[0].rm_eo - pmatch[0].rm_so + 1);
            if (pmatch[0].rm_so == pmatch[0].rm_eo)
            {
                current_pos++;
                if (*current_pos == '\0')
                {
                    break;
                }
            }
            else
            {
                current_pos += pmatch[0].rm_eo;
            }
        }
        if (!flag2)
        {
            printf("Нет совпадений.\n");
            free(curr_line);
            fclose(file);
            regfree(&regex);
            return 0;
        }
        char *result = (char *)malloc(new_len + 1);
        if (!result)
        {
            printf("Не удалось выделить память.\n");
            free(curr_line);
            fclose(file);
            regfree(&regex);
            return 0;
        }

        current_pos = curr_line;
        char *result_pos = result;
        while (regexec(&regex, current_pos, 1, pmatch, 0) == 0)
        {
            int before_match_len = pmatch[0].rm_so;
            strncpy(result_pos, current_pos, before_match_len);
            result_pos += before_match_len;

            if (pmatch[0].rm_so == pmatch[0].rm_eo)
            {
                current_pos++;
                if (*current_pos == '\0')
                {
                    break;
                }
            }
            else
            {
                current_pos += pmatch[0].rm_eo;
            }
        }

        strcpy(result_pos, current_pos);
        mass_of_lines[i++] = result;
    }

    free(curr_line);
    fclose(file);
    regfree(&regex);
    rewrite_file(mass_of_lines, name_file, count);
    if (flag1 == -1 && flag2 != 1)
    {
        printf("Файл пуст или ошибка чтения.\n");
        return 0;
    }
    return 1;
}

bool check_prefix(char *pattern, char *name_file)
{
    if (strncmp(pattern, "s/", 2) != 0)
    {
        printf("Строка не начинается с 's/'.\n");
        return 0;
    }

    const char *scnd_slash = strchr(pattern + 2, '/');
    if (!scnd_slash)
    {
        printf("Не найдена второй '/'.\n");
        return 0;
    }

    const char *thrd_slash = strchr(scnd_slash + 1, '/');
    if (!thrd_slash)
    {
        printf("Не найден третий '/'.\n");
        return 0;
    }
    if (pattern[thrd_slash - pattern + 1] != '\0')
    {
        printf("%c", pattern[thrd_slash - pattern + 1]);
        printf("После третьего '/' в строке есть что-то еще.\n");
        return 0;
    }
    const char *frst_slash = strchr(pattern, '/');

    size_t prefix_len = thrd_slash - (scnd_slash + 1);
    char prefix[prefix_len + 1];
    strncpy(prefix, scnd_slash + 1, prefix_len);
    prefix[prefix_len] = '\0';

    FILE *file = fopen(name_file, "r");
    if (!file)
    {
        printf("Ошибка открытия файла.\n");
        return 0;
    }
    int count = count_lines_in_file(name_file);
    if (count == 0)
    {
        printf("Файл пустой.\n");
        return 0;
    }
    char **mass_of_lines = (char **)calloc(count, sizeof(char *));
    char *curr_line = NULL;
    size_t len_line = 0;
    int flag1, flag2;
    unsigned int i = 0;

    while ((flag1 = getline(&curr_line, &len_line, file)) != -1)
    {
        flag2 = 1;
        size_t new_len = len_line;
        char *result = (char *)malloc(new_len + prefix_len + 1);
        if (!result)
        {
            printf("Не удалось выделить память.\n");
            free(curr_line);
            fclose(file);
            return 0;
        }
        strncpy(result, prefix, prefix_len);
        strcpy(result + prefix_len, curr_line);
        mass_of_lines[i++] = result;
    }

    free(curr_line);
    fclose(file);
    rewrite_file(mass_of_lines, name_file, count);
    if (flag1 == -1 && flag2 != 1)
    {
        printf("Файл пуст или ошибка чтения.\n");
        return 0;
    }
    return 1;
}

bool check_suffix(char *pattern, char *name_file)
{
    if (strncmp(pattern, "s/", 2) != 0)
    {
        printf("Строка не начинается с 's/'.\n");
        return 0;
    }

    const char *scnd_slash = strchr(pattern + 2, '/');
    if (!scnd_slash)
    {
        printf("Не найдена второй '/'.\n");
        return 0;
    }

    const char *thrd_slash = strchr(scnd_slash + 1, '/');
    if (!thrd_slash)
    {
        printf("Не найден третий '/'.\n");
        return 0;
    }
    if (pattern[thrd_slash - pattern + 1] != '\0')
    {
        printf("%c", pattern[thrd_slash - pattern + 1]);
        printf("После третьего '/' в строке есть что-то еще.\n");
        return 0;
    }
    const char *frst_slash = strchr(pattern, '/');

    size_t suffix_len = thrd_slash - (scnd_slash + 1);
    char suffix[suffix_len + 1];
    strncpy(suffix, scnd_slash + 1, suffix_len);
    suffix[suffix_len] = '\0';

    FILE *file = fopen(name_file, "r");
    if (!file)
    {
        printf("Ошибка открытия файла.\n");
        return 0;
    }
    int count = count_lines_in_file(name_file);
    if (count == 0)
    {
        printf("Файл пустой.\n");
        return 0;
    }
    char **mass_of_lines = (char **)calloc(count, sizeof(char *));
    char *curr_line = NULL;
    size_t len_line = 0;
    int flag1, flag2, flag3;
    unsigned int i = 0;

    while ((flag1 = getline(&curr_line, &len_line, file)) != -1)
    {
        flag2 = 1;
        flag3 = 0;
        if (curr_line[flag1 - 1] == '\n')
        {
            flag3 = 1;
        }
        char *result = (char *)malloc(len_line + suffix_len);
        if (!result)
        {
            printf("Не удалось выделить память.\n");
            free(curr_line);
            fclose(file);
            return 0;
        }
        memcpy(result, curr_line, flag1 - flag3);
        memcpy(result + (flag1 - flag3), suffix, suffix_len);
        if (flag3 == 1)
        {
            result[flag1 - flag3 + suffix_len] = '\n';
            result[flag1 - flag3 + suffix_len + 1] = '\0';
        }
        else
        {
            result[flag1 - flag3 + suffix_len] = '\0';
        }
        mass_of_lines[i++] = result;
    }
    free(curr_line);
    fclose(file);
    rewrite_file(mass_of_lines, name_file, count);
    if (flag1 == -1 && flag2 != 1)
    {
        printf("Файл пуст или ошибка чтения.\n");
        return 0;
    }
    return 1;
}
