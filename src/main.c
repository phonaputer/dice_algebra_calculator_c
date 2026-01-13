#include "dice_error.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>

void print_tokens_to_debug(TokenIterator *tokeit)
{
  Token token;
  while (tokeit_next(tokeit, &token))
  {
    switch (token.tokenType)
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
      printf("int: %d\n", token.integerValue);
      break;
    }
  }
  tokeit_rewind(tokeit);
}

void get_user_input(char *userInput, size_t size)
{
  fgets(userInput, size, stdin);

  size_t len = strlen(userInput);
  if (len > 0 && userInput[len - 1] == '\n')
  {
    userInput[len - 1] = '\0';
  }
}

ResultCode run(DErr **err)
{
  printf("Please enter a roll: ");

  char userInput[50];
  get_user_input(userInput, sizeof(userInput));

  printf("You wrote: '%s'\n", userInput);

  TokenIterator tokeit;

  ResultCode resultCode = tokenize(userInput, &tokeit, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    tokeit_free(&tokeit);
    return resultCode;
  }

  print_tokens_to_debug(&tokeit);

  Tree *tree;
  resultCode = parse(&tokeit, &tree, err);

  printf("Duh parsed\n");

  if (resultCode != RESULT_CODE_SUCCESS)
  {
    tree_free(&tree);
    tokeit_free(&tokeit);
    return resultCode;
  }

  tokeit_free(&tokeit);
  tree_free(&tree);

  return RESULT_CODE_SUCCESS;
}

int main(void)
{
  DErr *err;

  ResultCode resultCode = run(&err);

  if (resultCode != RESULT_CODE_SUCCESS)
  {
    printf("An error has occurred!\n");
    printf(
        "DEBUG message: %s\nUSER message: %s\n",
        err->debugMessage,
        err->endUserMessage
    );
    derr_free(err);
  }

  return resultCode;
}
