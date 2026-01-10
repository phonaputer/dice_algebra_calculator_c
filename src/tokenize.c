#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include "tokenize.h"
#include "global_error.h"

static int str_2_int(char *string)
{
    char *end;
    if (string[0] == '\0')
    {
        set_error("Can't convert empty string to int, tokenize.c, str_2_int");
        return 0;
    }

    errno = 0;
    long l = strtol(string, &end, 10);

    if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
    {
        set_error("Exceeded int max, tokenize.c, str_2_int");
        return 0;
    }

    if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
    {
        set_error("Exceeded int min, tokenize.c, str_2_int");
        return 0;
    }

    if (*end != '\0')
    {
        set_error("Failed to convert full string to long, tokenize.c, str_2_int");
        return 0;
    }

    return l;
}

typedef struct
{
    char *string;
    size_t size;
    size_t cap;
} OngoingInt;

static OngoingInt *ogint_new()
{
    OngoingInt *ogint = (OngoingInt *)malloc(sizeof(OngoingInt));
    if (ogint == NULL)
    {
        set_error("Failed to malloc ogint in tokenize.c, ogint_new.");
        return NULL;
    }

    ogint->size = 0;
    ogint->cap = 5 * sizeof(char);

    ogint->string = (char *)malloc(ogint->cap);
    if (ogint->string == NULL)
    {
        set_error("Failed to malloc ogint->string in tokenize.c, ogint_new.");
        return NULL;
    }

    return ogint;
}

static void ogint_free(OngoingInt *ogint)
{
    free(ogint->string);
    free(ogint);
}

static void ogint_append_char(OngoingInt *ogint, char c)
{
    if (ogint->size + sizeof(char) >= ogint->cap)
    {
        printf("string: %s, cap: %zu, size: %zu\n", ogint->string, ogint->cap, ogint->size);

        ogint->cap *= 2;
        char *temp = (char *)realloc(ogint->string, ogint->cap);
        if (temp == NULL)
        {
            set_error("Failed to realloc in tokenize.c, ogint_append_char.");
            return;
        }
        ogint->string = temp;
    }

    ogint->string[ogint->size] = c;
    ogint->size += sizeof(char);
}

static bool ogint_finalize(OngoingInt *ogint, Token *out)
{
    if (ogint->size < 1)
    {
        return false;
    }

    int theInt = str_2_int(ogint->string);
    if (is_there_an_error())
    {
        return false;
    }

    ogint->size = 0;

    *out = (Token){.tokenType = TOKEN_TYPE_INTEGER, .integerValue = theInt};

    return true;
}

static void ta_append_token(TokenArray *ta, Token token, OngoingInt *ogint)
{
    if (ta->size >= ta->cap)
    {
        ta->cap *= 2;
        Token *temp = (Token *)realloc(ta->tokens, ta->cap);
        if (temp == NULL)
        {
            set_error("Failed to realloc in tokenize.c, ta_append_token.");
            return;
        }
        ta->tokens = temp;
    }

    printf("ogint cap: %zu\n", ogint->cap);
    ta->tokens[ta->size] = token;
    printf("ogint cap2: %zu\n", ogint->cap);
    ta->size += sizeof(Token);
    printf("ogint cap3: %zu\n", ogint->cap);
}

static TokenArray *ta_new()
{
    TokenArray *ta = (TokenArray *)malloc(sizeof(TokenArray));
    if (ta == NULL)
    {
        set_error("Failed to malloc ta in tokenize.c, ta_new.");
        return NULL;
    }

    ta->size = 0;
    ta->cap = 5 * sizeof(Token);

    ta->tokens = (Token *)malloc(ta->cap);
    if (ta->tokens == NULL)
    {
        set_error("Failed to malloc ta->tokens in tokenize.c, ta_new.");
        free(ta);
        return NULL;
    }

    return ta;
}

void ta_free(TokenArray *ta)
{
    free(ta->tokens);
    free(ta);
}

TokenArray *tokenize(char input[])
{
    TokenArray *ta = ta_new();

    OngoingInt *ogint = ogint_new();

    bool finishedToken = false;
    TokenType tokenType;

    size_t inputLength = strlen(input);
    for (size_t i = 0; i < inputLength; i++)
    {
        char inputChar = input[i];

        switch (inputChar)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            ogint_append_char(ogint, inputChar);
            break;
        case 'd':
        case 'D':
            tokenType = TOKEN_TYPE_D;
            finishedToken = true;
            break;
        case 'l':
        case 'L':
            tokenType = TOKEN_TYPE_L;
            finishedToken = true;
            break;
        case 'h':
        case 'H':
            tokenType = TOKEN_TYPE_H;
            finishedToken = true;
            break;
        case '+':
            tokenType = TOKEN_TYPE_PLUS;
            finishedToken = true;
            break;
        case '-':
            tokenType = TOKEN_TYPE_MINUS;
            finishedToken = true;
            break;
        case '/':
            tokenType = TOKEN_TYPE_DIVIDE;
            finishedToken = true;
            break;
        case '*':
            tokenType = TOKEN_TYPE_MULTIPLY;
            finishedToken = true;
            break;
        case '(':
            tokenType = TOKEN_TYPE_OPEN_PAREN;
            finishedToken = true;
            break;
        case ')':
            tokenType = TOKEN_TYPE_CLOSE_PAREN;
            finishedToken = true;
            break;
        }

        if (!finishedToken)
        {
            continue;
        }

        finishedToken = false;

        Token intToken;
        bool haveIntToken = ogint_finalize(ogint, &intToken);
        if (is_there_an_error())
        {
            ta_free(ta);
            ogint_free(ogint);
            return NULL;
        };

        if (haveIntToken)
        {
            ta_append_token(ta, intToken, ogint);
            if (is_there_an_error())
            {
                ta_free(ta);
                ogint_free(ogint);
                return NULL;
            }
        }

        Token newToken = {.tokenType = tokenType, .integerValue = 0};

        ta_append_token(ta, newToken, ogint); // PROBLEM IS HERE
        if (is_there_an_error())
        {
            printf("Is this happening?\n");

            ta_free(ta);
            ogint_free(ogint);
            return NULL;
        }
    }

    if (ogint->size > 0)
    {
        Token intToken;
        bool haveIntToken = ogint_finalize(ogint, &intToken);
        if (is_there_an_error())
        {
            ta_free(ta);
            ogint_free(ogint);
            return NULL;
        }

        if (haveIntToken)
        {
            ta_append_token(ta, intToken, ogint);
            if (is_there_an_error())
            {
                ta_free(ta);
                ogint_free(ogint);
                return NULL;
            }
        }
    }

    ogint_free(ogint);

    return ta;
}