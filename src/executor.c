
#include "executor.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

ResultCode execute_shortroll(Tree *tree, int *result)
{
  unsigned int faces = tree->nodeData.shortRoll.faces;
  if (faces < 1)
  {
    *result = 0;
    return RESULT_CODE_SUCCESS;
  }

  printf("\nRolling d%d...\n", faces);

  int randomNumber = rand();

  *result = randomNumber % faces + 1;

  printf("You rolled: %d\n", *result);

  return RESULT_CODE_SUCCESS;
}

int compare_integers(const void *a, const void *b)
{
  return (*(int *)a - *(int *)b);
}

ResultCode execute_longroll(Tree *tree, int *result)
{
  unsigned int die = tree->nodeData.longRoll.die;
  unsigned int faces = tree->nodeData.longRoll.faces;
  unsigned int *high = tree->nodeData.longRoll.high;
  unsigned int *low = tree->nodeData.longRoll.low;

  if (die < 1 || faces < 1)
  {
    *result = 0;
    return RESULT_CODE_SUCCESS;
  }

  printf("\nRolling %dd%d...\n", die, faces);

  int sum = 0;
  int rolls[die];

  for (unsigned int i = 0; i < die; i++)
  {
    int randomNumber = rand();

    rolls[i] = randomNumber % faces + 1;
    sum += rolls[i];

    printf("You rolled: %d\n", rolls[i]);
  }

  if (high == NULL && low == NULL)
  {
    *result = sum;
    return RESULT_CODE_SUCCESS;
  }

  size_t arrSize = sizeof(rolls) / sizeof(int);
  qsort(rolls, arrSize, sizeof(int), compare_integers);

  if (high != NULL)
  {
    if (*high < die)
    {
      sum = 0;
      for (unsigned int i = die - *high; i < die; i++)
      {
        sum += rolls[i];
      }
    }
  }
  else if (low != NULL)
  {
    if (*low < die)
    {
      sum = 0;
      for (unsigned int i = 0; i < *low; i++)
      {
        sum += rolls[i];
      }
    }
  }

  *result = sum;

  return RESULT_CODE_SUCCESS;
}

ResultCode execute_math(Tree *tree, int *result, DErr **err)
{

  int leftResult;
  ResultCode resultCode = execute(tree->nodeData.math.l, &leftResult, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    return resultCode;
  }

  int rightResult;
  resultCode = execute(tree->nodeData.math.r, &rightResult, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    return resultCode;
  }

  int mathResult = 0;

  switch (tree->nodeData.math.op)
  {
  case MATH_OP_ADD:
    mathResult = leftResult + rightResult;
    break;
  case MATH_OP_SUBTRACT:
    mathResult = leftResult - rightResult;
    break;
  case MATH_OP_MULTIPLY:
    mathResult = leftResult * rightResult;
    break;
  case MATH_OP_DIVIDE:
    if (rightResult == 0)
    {
      derr_set(
          err,
          "Division by zero in executor.c, execute_math",
          "Divison by zero is not allowed."
      );
      return RESULT_CODE_INTERNAL_ERROR;
    }
    else
    {
      mathResult = leftResult / rightResult;
    }
    break;
  default:
    derr_set(
        err,
        "Unexpected mathematical operation in executor.c, execute_math",
        UNEXPECTED_ERR_MSG
    );
    return RESULT_CODE_INTERNAL_ERROR;
  }

  *result = mathResult;

  return RESULT_CODE_SUCCESS;
}

ResultCode execute_node(Tree *tree, int *result, DErr **err)
{
  switch (tree->nodeType)
  {
  case NODE_TYPE_INTEGER:
    *result = tree->nodeData.integer.integer;

    return RESULT_CODE_SUCCESS;

  case NODE_TYPE_LONGROLL:
    return execute_longroll(tree, result);

  case NODE_TYPE_SHORTROLL:
    return execute_shortroll(tree, result);

  case NODE_TYPE_MATH:
    return execute_math(tree, result, err);

  default:
    derr_set(
        err,
        "Unexpected node type in executor.c, execute_node",
        UNEXPECTED_ERR_MSG
    );
    return RESULT_CODE_INTERNAL_ERROR;
  }
}

ResultCode execute(Tree *tree, int *result, DErr **err)
{
  srand(time(NULL));

  return execute_node(tree, result, err);
}
