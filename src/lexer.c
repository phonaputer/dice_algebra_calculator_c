#include "lexer.h"
#include "global_error.h"
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static int str_2_int(char *string)
{
  char *end;
  if (string[0] == '\0')
  {
    set_error("Can't convert empty string to int, lexer.c, str_2_int");
    return 0;
  }

  errno = 0;
  long l = strtol(string, &end, 10);

  if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
  {
    set_error("Exceeded int max, lexer.c, str_2_int");
    return 0;
  }

  if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
  {
    set_error("Exceeded int min, lexer.c, str_2_int");
    return 0;
  }

  if (*end != '\0')
  {
    set_error("Failed to convert full string to long, lexer.c, str_2_int");
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
    set_error("Failed to malloc ogint in lexer.c, ogint_new.");
    return NULL;
  }

  ogint->size = 0;
  ogint->cap = 5;

  ogint->string = (char *)malloc(ogint->cap * sizeof(char));
  if (ogint->string == NULL)
  {
    set_error("Failed to malloc ogint->string in lexer.c, ogint_new.");
    return NULL;
  }

  return ogint;
}

static void ogint_free(OngoingInt *ogint)
{
  if (ogint == NULL) return;

  if (ogint->string != NULL) free(ogint->string);

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
      set_error("Failed to realloc in lexer.c, ogint_append_char.");
      return;
    }
    ogint->string = temp;
  }

  ogint->string[ogint->size] = c;
  ogint->size++;
}

static bool ogint_finalize(OngoingInt *ogint, Token *out)
{
  if (ogint->size < 1) return false;

  int theInt = str_2_int(ogint->string);
  if (is_there_an_error()) return false;

  ogint->size = 0;

  *out = (Token){
      .tokenType = TOKEN_TYPE_INTEGER,
      .integerValue = theInt,
  };

  return true;
}

typedef struct
{
  Token *tokens;
  unsigned int size;
  unsigned int cap;
} TokenArray;

static void ta_append_token(TokenArray *ta, Token token)
{
  if (ta->size >= ta->cap)
  {
    ta->cap *= 2;
    Token *temp = (Token *)realloc(ta->tokens, ta->cap * sizeof(Token));
    if (temp == NULL)
    {
      set_error("Failed to realloc in lexer.c, ta_append_token.");
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
    set_error("Failed to malloc ta in lexer.c, ta_new.");
    return NULL;
  }

  ta->size = 0;
  ta->cap = 5;

  ta->tokens = (Token *)malloc(ta->cap * sizeof(Token));
  if (ta->tokens == NULL)
  {
    set_error("Failed to malloc ta->tokens in lexer.c, ta_new.");
    free(ta);
    return NULL;
  }

  return ta;
}

void ta_free(TokenArray *ta)
{
  if (ta == NULL) return;

  if (ta->tokens != NULL) free(ta->tokens);

  free(ta);
}

// tokeit_copy_from_ta creates a new TokenIterator from a TokenArray.
//
// This includes transferring ownership of the heap-allocated data within "ta"
// to "out".
void tokeit_copy_from_ta(TokenArray *ta, TokenIterator *out)
{
  if (out == NULL)
  {
    set_error("Output pointer is null in lexer.c, tokens_copy_from_ta");
    return;
  }

  *out = (TokenIterator){
      ._curToken = 0,
      ._size = ta->size,
      ._tokenArray = ta->tokens,
  };

  ta->tokens = NULL;
}

bool tokeit_next(TokenIterator *tokens, Token **out)
{
  if (tokens->_curToken >= tokens->_size) return false;

  *out = &tokens->_tokenArray[tokens->_curToken];

  tokens->_curToken++;
}

bool tokeit_peek(TokenIterator *tokens, Token **out)
{
  int peekIdx = tokens->_curToken + 1;

  if (peekIdx >= tokens->_size) return false;

  *out = &tokens->_tokenArray[peekIdx];
}

bool tokeit_free(TokenIterator *tokeit)
{
  if (tokeit == NULL) return;

  if (tokeit->_tokenArray != NULL) free(tokeit->_tokenArray);

  free(tokeit);
}

void ogint_append_token_to_ta(TokenArray *ta, OngoingInt *ogint)
{
  Token intToken;
  bool haveIntToken = ogint_finalize(ogint, &intToken);
  if (is_there_an_error()) return;

  if (!haveIntToken) return;

  // No need to error check here because the caller of ogint_append_token_to_ta
  // will do that
  ta_append_token(ta, intToken);
}

void _tokenize(TokenArray *ta, OngoingInt *ogint, char input[])
{
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
      if (is_there_an_error()) return;
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
    case ' ':
    case '\t':
    case '\n':
      // whitespace is ignored.
      break;
    default:
      char errMsg[100];
      sprintf(errMsg, "Unexpected character in input: '%c'", inputChar);
      set_error(errMsg);
      return;
    }

    if (!finishedToken) continue;

    finishedToken = false;

    ogint_append_token_to_ta(ta, ogint);
    if (is_there_an_error()) return;

    ta_append_token(
        ta,
        (Token){
            .tokenType = tokenType,
            .integerValue = 0,
        }
    );
    if (is_there_an_error()) return;
  }

  // No need to error check here because the caller of _tokenize will do that
  ogint_append_token_to_ta(ta, ogint);
}

void tokenize(char input[], TokenIterator *out)
{
  TokenArray *ta = ta_new();
  if (is_there_an_error()) return;

  OngoingInt *ogint = ogint_new();
  if (is_there_an_error())
  {
    ta_free(ta);
    return;
  }

  _tokenize(ta, ogint, input);
  if (is_there_an_error())
  {
    ta_free(ta);
    ogint_free(ogint);
    return;
  }

  tokeit_copy_from_ta(ta, out);

  ta_free(ta);
  ogint_free(ogint);
}