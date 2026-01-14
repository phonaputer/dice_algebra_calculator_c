
#include "executor.h"
#include <stdlib.h>
#include <time.h>

ResultCode execute_shortroll(Tree *tree, int *result)
{
  int randomNumber = rand();

  *result = randomNumber % tree->nodeData.shortRoll.faces + 1;

  return RESULT_CODE_SUCCESS;
}

ResultCode execute_longroll(Tree *tree, int *result)
{
  int sum = 0;

  for (int i = 0; i < tree->nodeData.longRoll.die; i++)
  {
    int randomNumber = rand();

    sum += randomNumber % tree->nodeData.longRoll.faces + 1;
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
