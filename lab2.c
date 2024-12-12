#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <regex.h>
#include <stdlib.h>


void rewrite_file(char ** mass_of_lines, const char *filename,int count)
{
    FILE *file = fopen(filename,"w");
    for (int i = 0; i < count; i++)
    {
        fprintf(file,"%s",mass_of_lines[i]);
    }
}

int count_lines_in_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Ошибка при открытии файла");
        return -1; 
    }

    int line_count = 0;
    char ch;

    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') { 
            line_count++;
        }
    }

    fclose(file);
    return line_count;
}

bool check_replace(char *pattern, char *name_file) {
    if (strncmp(pattern, "s/", 2) != 0) {
        printf("Строка не начинается с 's/'.\n");
        return 0;
    }

    const char *scnd_slash = strchr(pattern + 2, '/');
    if (!scnd_slash) {
        printf("Не найдена второй '/'.\n");
        return 0;
    }

    const char *thrd_slash = strchr(scnd_slash + 1, '/');
    if (!thrd_slash) {
        printf("Не найден третий '/'.\n");
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
    if (reti) {
        printf("Ошибка компиляции регулярного выражения: %s\n", rgx);
        return 0;
    }

    FILE *file = fopen(name_file, "r+");
    if (!file) {
        printf("Ошибка открытия файла.\n");
        regfree(&regex);
        return 0;
    }
    int count = count_lines_in_file(name_file);
    if (count == -1)
    {
        return 0;
    }
    char **mass_of_lines = (char **)calloc(count,sizeof(char *));
    char *curr_line = NULL;
    size_t len_line = 0;
    int flag1, flag2,flag3;
    unsigned int i = 0;

    while ((flag1 = getline(&curr_line, &len_line, file)) != -1) {
        printf("Строка до замены: %s\n", curr_line);
        flag2 = 1;
        size_t new_len = len_line;
        char *current_pos = curr_line;

        while (regexec(&regex, current_pos, 1, pmatch, 0) == 0) {
            new_len += strlen(replacement) - (pmatch[0].rm_eo - pmatch[0].rm_so);
            current_pos += pmatch[0].rm_eo;
        }

        char *result = (char *)malloc(new_len + 1);
        if (!result) {
            printf("Не удалось выделить память.\n");
            free(curr_line);
            fclose(file);
            regfree(&regex);
            return 0;
        }

        current_pos = curr_line;
        char *result_pos = result;
        while (regexec(&regex, current_pos, 1, pmatch, 0) == 0) {
            int before_match_len = pmatch[0].rm_so;
            strncpy(result_pos, current_pos, before_match_len);
            result_pos += before_match_len;

            strcpy(result_pos, replacement);
            result_pos += strlen(replacement);
            current_pos += pmatch[0].rm_eo;
        }

        strcpy(result_pos, current_pos);
        mass_of_lines[i++] = result;
    }

    free(curr_line);
    fclose(file);
    regfree(&regex);
    rewrite_file(mass_of_lines,name_file,count);
    if (flag1 == -1 && flag2 != 1) {
        printf("Файл пуст или ошибка чтения.\n");
        return 0;
    }

    return 1;
}