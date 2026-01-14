
#include "executor.h"

ResultCode execute_shortroll(Tree *tree, int *result)
{
  *result = tree->nodeData.shortRoll.faces;

  return RESULT_CODE_SUCCESS;
}

ResultCode execute_longroll(Tree *tree, int *result)
{
  *result = tree->nodeData.longRoll.faces * tree->nodeData.longRoll.die;

  return RESULT_CODE_SUCCESS;
}

ResultCode execute_math(Tree *tree, int *result)
{

  int leftResult;
  ResultCode resultCode = execute(tree->nodeData.math.l, &leftResult);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    return resultCode;
  }

  int rightResult;
  resultCode = execute(tree->nodeData.math.r, &rightResult);
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
    mathResult = leftResult / rightResult;
    break;
  default:
    return RESULT_CODE_INTERNAL_ERROR;
  }

  *result = mathResult;

  return RESULT_CODE_SUCCESS;
}

ResultCode execute(Tree *tree, int *result)
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
    return execute_math(tree, result);

  default:
    return RESULT_CODE_INTERNAL_ERROR;
  }
}
