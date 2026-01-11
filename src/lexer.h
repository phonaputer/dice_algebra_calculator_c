#pragma once

#include <stdbool.h>
#include <string.h>

typedef enum
{
  TOKEN_TYPE_D,
  TOKEN_TYPE_H,
  TOKEN_TYPE_L,
  TOKEN_TYPE_PLUS,
  TOKEN_TYPE_MINUS,
  TOKEN_TYPE_MULTIPLY,
  TOKEN_TYPE_DIVIDE,
  TOKEN_TYPE_OPEN_PAREN,
  TOKEN_TYPE_CLOSE_PAREN,
  TOKEN_TYPE_INTEGER
} TokenType;

typedef struct
{
  TokenType tokenType;
  int integerValue;
} Token;

typedef struct
{
  Token *_tokenArray;
  unsigned int _size;
  unsigned int _curToken;
} TokenIterator;

// lol @ this abbreviation
bool tokeit_next(TokenIterator *tokens, Token *out);

bool tokeit_peek(TokenIterator *tokens, Token *out);

void tokenize(char input[], TokenIterator *out);