
#include "executor.h"

ResultCode executeShortRoll(Tree *tree, int *result)
{
  *result = tree->nodeData.shortRoll.faces;

  return RESULT_CODE_SUCCESS;
}

ResultCode executeLongRoll(Tree *tree, int *result)
{
  *result = tree->nodeData.longRoll.faces * tree->nodeData.longRoll.die;

  return RESULT_CODE_SUCCESS;
}

ResultCode executeMath(Tree *tree, int *result)
{
    

  switch (tree->nodeData.math.op)
  {
  case MATH_OP_ADD:
  case MATH_OP_SUBTRACT:
  case MATH_OP_MULTIPLY:
  case MATH_OP_DIVIDE:
  default:
    return RESULT_CODE_INTERNAL_ERROR;
  }
}

ResultCode execute(Tree *tree, int *result) {}
