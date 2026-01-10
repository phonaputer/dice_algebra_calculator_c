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
    unsigned int size;
    unsigned int cap;
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
    ogint->cap = 5;

    ogint->string = (char *)malloc(ogint->cap * sizeof(char));
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
        ogint->cap *= 2;
        char *temp = (char *)realloc(ogint->string, ogint->cap * sizeof(char));
        if (temp == NULL)
        {
            set_error("Failed to realloc in tokenize.c, ogint_append_char.");
            return;
        }
        ogint->string = temp;
    }

    ogint->string[ogint->size] = c;
    ogint->size++;
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

static void ta_append_token(TokenArray *ta, Token token)
{
    if (ta->size >= ta->cap)
    {
        ta->cap *= 2;
        Token *temp = (Token *)realloc(ta->tokens, ta->cap * sizeof(Token));
        if (temp == NULL)
        {
            set_error("Failed to realloc in tokenize.c, ta_append_token.");
            return;
        }
        ta->tokens = temp;
    }

    ta->tokens[ta->size] = token;
    ta->size++;
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
    ta->cap = 5;

    ta->tokens = (Token *)malloc(ta->cap * sizeof(Token));
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
            if (is_there_an_error())
            {
                ogint_free(ogint);
                ta_free(ta);
                return NULL;
            }
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
            ta_append_token(ta, intToken);
            if (is_there_an_error())
            {
                ta_free(ta);
                ogint_free(ogint);
                return NULL;
            }
        }

        Token newToken = {.tokenType = tokenType, .integerValue = 0};

        ta_append_token(ta, newToken);
        if (is_there_an_error())
        {
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
            ta_append_token(ta, intToken);
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