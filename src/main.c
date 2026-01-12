#include "dice_error.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
  printf("Please enter a roll: ");

  char userInput[50];

  fgets(userInput, sizeof(userInput), stdin);

  size_t len = strlen(userInput);
  if (len > 0 && userInput[len - 1] == '\n')
  {
    userInput[len - 1] = '\0';
  }

  printf("You wrote: '%s'\n", userInput);

  DErr *err;
  TokenIterator tokeit;

  ResultCode resultCode = tokenize(userInput, &tokeit, &err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    printf("An error has occurred\n");
    printf("DEBUG: %s\nUSER: %s\n", err->debugMessage, err->endUserMessage);
    derr_free(err);
    return resultCode;
  }

  Token *token = NULL;
  while (tokeit_next(&tokeit, &token))
  {
    switch (token->tokenType)
    {
    case TOKEN_TYPE_D:
      printf("d\n");
      break;
    case TOKEN_TYPE_H:
      printf("h\n");
      break;
    case TOKEN_TYPE_L:
      printf("l\n");
      break;
    case TOKEN_TYPE_PLUS:
      printf("+\n");
      break;
    case TOKEN_TYPE_MINUS:
      printf("-\n");
      break;
    case TOKEN_TYPE_MULTIPLY:
      printf("*\n");
      break;
    case TOKEN_TYPE_DIVIDE:
      printf("/\n");
      break;
    case TOKEN_TYPE_OPEN_PAREN:
      printf("(\n");
      break;
    case TOKEN_TYPE_CLOSE_PAREN:
      printf(")\n");
      break;
    case TOKEN_TYPE_INTEGER:
      printf("int: %d\n", token->integerValue);
      break;
    }
  }

  tokeit_free(&tokeit);

  return 0;
}
