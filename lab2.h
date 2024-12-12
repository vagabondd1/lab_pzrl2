#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool check_replace(char *pattern, char *name_file);
bool check_remove(char *str);
bool check_suffix(char *str);
bool check_prefix(char *str);
void rewrite_file(char ** mass_of_lines, FILE* file);
int count_lines_in_file(const char *filename);