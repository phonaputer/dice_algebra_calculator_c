#pragma once

#include <string.h>

typedef struct
{
    char *token;
    size_t token;
} Token;

typedef struct
{
    Token *tokens;
    size_t size;
} TokenArray;

TokenArray tokenize(char input[]);