#include "dice_error.h"
#include "executor.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>

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
  printf("Please enter a dice algebra expression: ");

  char userInput[100];
  get_user_input(userInput, sizeof(userInput));

  TokenIterator tokeit;

  ResultCode resultCode = tokenize(userInput, &tokeit, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    tokeit_free(&tokeit);
    return derr_add_trace(resultCode, err, "tokenize: ");
  }

  Tree *tree;
  resultCode = parse(&tokeit, &tree, err);
  tokeit_free(&tokeit);

  if (resultCode != RESULT_CODE_SUCCESS)
  {
    return derr_add_trace(resultCode, err, "parse: ");
  }

  int result;
  resultCode = execute(tree, &result, err);
  tree_free(&tree);

  if (resultCode != RESULT_CODE_SUCCESS)
  {
    return derr_add_trace(resultCode, err, "execute: ");
  }

  printf("Roll result is: %d\n", result);

  return RESULT_CODE_SUCCESS;
}

int main(void)
{
  DErr *err;

  ResultCode resultCode = run(&err);

  if (resultCode != RESULT_CODE_SUCCESS)
  {
    printf("ERROR: %s\n", err->endUserMessage->str);

#ifdef DEBUG
    printf("DEBUG message: %s\n", err->debugMessage->str);
#endif

    derr_free(&err);
  }

  return resultCode;
}
