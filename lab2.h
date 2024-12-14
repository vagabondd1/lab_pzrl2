#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool check_replace(char *pattern, char *name_file);
bool check_remove(char *pattern,char *name_file);
bool check_suffix(char *pattern,char *name_file);
bool check_prefix(char *pattern,char *name_file);
void rewrite_file(char ** mass_of_lines, FILE* file);
size_t count_lines_in_file(const char *filename);