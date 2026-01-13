#pragma once

#include "dice_error.h"
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
bool tokeit_next(TokenIterator *tokeit, Token *out);

bool tokeit_peek(TokenIterator *tokeit, Token *out);

bool tokeit_peek_next(TokenIterator *tokeit, Token *out);

void tokeit_rewind(TokenIterator *tokeit);

void tokeit_free(TokenIterator *tokeit);

bool tokeit_has_next(TokenIterator *tokeit);

ResultCode tokenize(char input[], TokenIterator *out, DErr **err);
