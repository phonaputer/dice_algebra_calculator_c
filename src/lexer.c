#include "lexer.h"
#include "dice_error.h"
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ResultCode str_2_int(char *string, int *out, DErr **err)
{
  char *end;
  if (string[0] == '\0')
  {
    derr_set(
        err,
        "Can't convert empty string to int, lexer.c, str_2_int",
        "An unexpected error has occurred."
    );
    return RESULT_CODE_INTERNAL_ERROR;
  }

  errno = 0;
  long l = strtol(string, &end, 10);

  if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
  {
    derr_set(
        err,
        "Exceeded int max, lexer.c, str_2_int",
        "An unexpected error has occurred."
    );
    return RESULT_CODE_INTERNAL_ERROR;
  }

  if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
  {
    derr_set(
        err,
        "Exceeded int min, lexer.c, str_2_int",
        "An unexpected error has occurred."
    );
    return RESULT_CODE_INTERNAL_ERROR;
  }

  if (*end != '\0')
  {
    derr_set(
        err,
        "Failed to convert full string to long, lexer.c, str_2_int",
        "An unexpected error has occurred."
    );
    return RESULT_CODE_INTERNAL_ERROR;
  }

  *out = l;

  return RESULT_CODE_SUCCESS;
}

typedef struct
{
  char *string;
  unsigned int size;
  unsigned int cap;
} OngoingInt;

static ResultCode ogint_new(OngoingInt **out, DErr **err)
{
  *out = (OngoingInt *)malloc(sizeof(OngoingInt));
  if (*out == NULL)
  {
    derr_set(
        err,
        "Failed to malloc OngoingInt in lexer.c, ogint_new.",
        "An unexpected error has occurred."
    );
    return RESULT_CODE_INTERNAL_ERROR;
  }

  (*out)->size = 0;
  (*out)->cap = 5;

  (*out)->string = (char *)malloc((*out)->cap * sizeof(char));
  if ((*out)->string == NULL)
  {
    derr_set(
        err,
        "Failed to malloc OngoingInt->string in lexer.c, ogint_new.",
        "An unexpected error has occurred."
    );
    free(*out);
    return RESULT_CODE_INTERNAL_ERROR;
  }

  return RESULT_CODE_SUCCESS;
}

static void ogint_free(OngoingInt *ogint)
{
  if (ogint == NULL) return;

  if (ogint->string != NULL)
  {
    free(ogint->string);
  }

  free(ogint);
}

static ResultCode ogint_append_char(OngoingInt *ogint, char c, DErr **err)
{
  if (ogint->size >= ogint->cap)
  {
    ogint->cap *= 2;
    char *temp = (char *)realloc(ogint->string, ogint->cap * sizeof(char));
    if (temp == NULL)
    {
      derr_set(
          err,
          "Failed to realloc in lexer.c, ogint_append_char.",
          "An unexpected error has occurred."
      );
      return RESULT_CODE_INTERNAL_ERROR;
    }

    ogint->string = temp;

    return RESULT_CODE_SUCCESS;
  }

  ogint->string[ogint->size] = c;
  ogint->size++;

  return RESULT_CODE_SUCCESS;
}

static ResultCode ogint_finalize(
    OngoingInt *ogint, Token *tokenOut, bool *haveTokenOut, DErr **err
)
{
  if (ogint->size < 1)
  {
    *haveTokenOut = false;
    return RESULT_CODE_SUCCESS;
  }

  ResultCode resultCode = ogint_append_char(ogint, '\0', err);
  if (resultCode != RESULT_CODE_SUCCESS) return resultCode;

  int theInt;
  resultCode = str_2_int(ogint->string, &theInt, err);
  if (resultCode != RESULT_CODE_SUCCESS) return resultCode;

  ogint->size = 0;

  *tokenOut = (Token){
      .tokenType = TOKEN_TYPE_INTEGER,
      .integerValue = theInt,
  };

  *haveTokenOut = true;

  return RESULT_CODE_SUCCESS;
}

typedef struct
{
  Token *tokens;
  unsigned int size;
  unsigned int cap;
} TokenArray;

static ResultCode ta_append_token(TokenArray *ta, Token token, DErr **err)
{
  if (ta->size >= ta->cap)
  {
    ta->cap *= 2;
    Token *temp = (Token *)realloc(ta->tokens, ta->cap * sizeof(Token));
    if (temp == NULL)
    {

      derr_set(
          err,
          "Failed to realloc in lexer.c, ta_append_token.",
          "An unexpected error has occurred."
      );
      return RESULT_CODE_INTERNAL_ERROR;
    }
    ta->tokens = temp;
  }

  ta->tokens[ta->size] = token;
  ta->size++;

  return RESULT_CODE_SUCCESS;
}

static ResultCode ta_new(TokenArray **out, DErr **err)
{
  *out = (TokenArray *)malloc(sizeof(TokenArray));
  if (*out == NULL)
  {
    derr_set(
        err,
        "Failed to malloc TokenArray in lexer.c, ta_new.",
        "An unexpected error has occurred."
    );
    return RESULT_CODE_INTERNAL_ERROR;
  }

  (*out)->size = 0;
  (*out)->cap = 5;

  (*out)->tokens = (Token *)malloc((*out)->cap * sizeof(Token));
  if ((*out)->tokens == NULL)
  {
    derr_set(
        err,
        "Failed to malloc TokenArray->tokens in lexer.c, ta_new.",
        "An unexpected error has occurred."
    );
    free((*out));
    return RESULT_CODE_INTERNAL_ERROR;
  }

  return RESULT_CODE_SUCCESS;
}

void ta_free(TokenArray *ta)
{
  if (ta == NULL) return;

  if (ta->tokens != NULL)
  {
    free(ta->tokens);
  }

  free(ta);
}

// tokeit_copy_from_ta creates a new TokenIterator from a TokenArray.
//
// This includes transferring ownership of the heap-allocated data within the
// TokenArray to the resulting TokenIterator.
ResultCode tokeit_copy_from_ta(TokenArray *ta, TokenIterator *out, DErr **err)
{
  if (out == NULL)
  {
    derr_set(
        err,
        "Output TokenIterator pointer is null in lexer.c, tokeit_copy_from_ta.",
        "An unexpected error has occurred."
    );
    return RESULT_CODE_INTERNAL_ERROR;
  }

  *out = (TokenIterator){
      ._curToken = 0,
      ._size = ta->size,
      ._tokenArray = ta->tokens,
  };

  ta->tokens = NULL;

  return RESULT_CODE_SUCCESS;
}

bool tokeit_next(TokenIterator *tokens, Token **out)
{
  if (tokens->_curToken >= tokens->_size) return false;

  *out = &tokens->_tokenArray[tokens->_curToken];

  tokens->_curToken++;

  return true;
}

bool tokeit_peek(TokenIterator *tokens, Token **out)
{
  unsigned int peekIdx = tokens->_curToken + 1;

  if (peekIdx >= tokens->_size) return false;

  *out = &tokens->_tokenArray[peekIdx];

  return true;
}

void tokeit_free(TokenIterator *tokeit)
{
  if (tokeit->_tokenArray != NULL)
  {
    free(tokeit->_tokenArray);
  }
}

ResultCode
ogint_append_token_to_ta(TokenArray *ta, OngoingInt *ogint, DErr **err)
{
  Token intToken;
  bool haveIntToken;
  ResultCode resultCode = ogint_finalize(ogint, &intToken, &haveIntToken, err);
  if (resultCode != RESULT_CODE_SUCCESS) return resultCode;

  if (!haveIntToken) return RESULT_CODE_SUCCESS;

  resultCode = ta_append_token(ta, intToken, err);
  if (resultCode != RESULT_CODE_SUCCESS) return resultCode;

  return RESULT_CODE_SUCCESS;
}

ResultCode
_tokenize(TokenArray *ta, OngoingInt *ogint, char input[], DErr **err)
{
  ResultCode resultCode;
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
      resultCode = ogint_append_char(ogint, inputChar, err);
      if (resultCode != RESULT_CODE_SUCCESS) return resultCode;
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
    default:;
      char errMsg[100];
      sprintf(errMsg, "Unexpected character in input: '%c'", inputChar);
      derr_set(err, errMsg, errMsg);
      return RESULT_CODE_INTERNAL_ERROR;
    }

    if (!finishedToken) continue;

    finishedToken = false;

    resultCode = ogint_append_token_to_ta(ta, ogint, err);
    if (resultCode != RESULT_CODE_SUCCESS) return resultCode;

    resultCode = ta_append_token(
        ta,
        (Token){
            .tokenType = tokenType,
            .integerValue = 0,
        },
        err
    );
    if (resultCode != RESULT_CODE_SUCCESS) return resultCode;
  }

  resultCode = ogint_append_token_to_ta(ta, ogint, err);
  if (resultCode != RESULT_CODE_SUCCESS) return resultCode;

  return RESULT_CODE_SUCCESS;
}

ResultCode tokenize(char input[], TokenIterator *out, DErr **err)
{
  TokenArray *ta;
  ResultCode resultCode = ta_new(&ta, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    return resultCode;
  }

  OngoingInt *ogint;
  resultCode = ogint_new(&ogint, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    ta_free(ta);
    return resultCode;
  }

  resultCode = _tokenize(ta, ogint, input, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    ta_free(ta);
    ogint_free(ogint);
    return resultCode;
  }

  resultCode = tokeit_copy_from_ta(ta, out, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    ogint_free(ogint);
    ta_free(ta);
    return resultCode;
  }

  ogint_free(ogint);
  ta_free(ta);

  return RESULT_CODE_SUCCESS;
}
