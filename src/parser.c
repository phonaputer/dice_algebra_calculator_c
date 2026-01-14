#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

ResultCode validate_parenthesis(TokenIterator *tokeit, DErr **err)
{
  int openCount = 0;
  int closedCount = 0;

  Token token;
  while (tokeit_next(tokeit, &token))
  {
    if (token.tokenType == TOKEN_TYPE_OPEN_PAREN)
    {
      openCount++;
    }
    else if (token.tokenType == TOKEN_TYPE_CLOSE_PAREN)
    {
      closedCount++;
    }
  }
  tokeit_rewind(tokeit);

  if (openCount != closedCount)
  {
    derr_set(
        err,
        "User input does not contain matching parentheses.",
        "Input contains unclosed parethesis."
    );
    return RESULT_CODE_INTERNAL_ERROR;
  }

  return RESULT_CODE_SUCCESS;
}

void tree_free(Tree **tree)
{
  if (tree == NULL) return;

  Tree *theTree = *tree;

  if (theTree == NULL) return;

  if (theTree->nodeType == NODE_TYPE_MATH)
  {
    tree_free(&theTree->nodeData.math.l);
    theTree->nodeData.math.l = NULL;

    tree_free(&theTree->nodeData.math.r);
    theTree->nodeData.math.r = NULL;
  }

  free(theTree);
  *tree = NULL;
}

ResultCode tree_new(Tree **out, DErr **err, NodeType nodeType)
{
  Tree *newTree = (Tree *)malloc(sizeof(Tree));
  if (newTree == NULL)
  {
    *out = NULL;
    derr_set(
        err, "Failed to malloc Tree in parser.c, tree_new.", UNEXPECTED_ERR_MSG
    );
    return RESULT_CODE_INTERNAL_ERROR;
  }

  *newTree = (Tree){.nodeType = nodeType};
  *out = newTree;

  return RESULT_CODE_SUCCESS;
}

ResultCode consume_d(TokenIterator *tokeit, DErr **err)
{
  Token token;
  bool haveToken = tokeit_next(tokeit, &token);
  if (!haveToken)
  {
    derr_set(
        err,
        "Unexpected end of input getting D token in parser.c, consume_d",
        "Unexpected end of input."
    );
    return RESULT_CODE_INTERNAL_ERROR;
  }
  if (token.tokenType != TOKEN_TYPE_D)
  {
    derr_set(err, "Expected D token in parser.c, consume_d", "Invalid input.");
    return RESULT_CODE_INTERNAL_ERROR;
  }

  return RESULT_CODE_SUCCESS;
}

ResultCode parse_integer_raw(TokenIterator *tokeit, int *out, DErr **err)
{
  Token token;
  bool haveToken = tokeit_next(tokeit, &token);

  if (!haveToken)
  {
    derr_set(
        err,
        "Unexpected end of input in parser.c, parse_integer_raw",
        "Unexpected end of input."
    );
    return RESULT_CODE_INTERNAL_ERROR;
  }
  if (token.tokenType != TOKEN_TYPE_INTEGER)
  {
    derr_set(
        err,
        "Expected int token in parser.c, parse_integer_raw",
        "Invalid input."
    );
    return RESULT_CODE_INTERNAL_ERROR;
  }

  *out = token.integerValue;

  return RESULT_CODE_SUCCESS;
}

ResultCode parse_shortroll(TokenIterator *tokeit, Tree **out, DErr **err)
{
  ResultCode resultCode = consume_d(tokeit, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    return derr_add_trace(resultCode, err, "parse_shortroll: ");
  }

  int faces;
  resultCode = parse_integer_raw(tokeit, &faces, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    return derr_add_trace(resultCode, err, "parse_shortroll: ");
  }

  resultCode = tree_new(out, err, NODE_TYPE_SHORTROLL);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    return derr_add_trace(resultCode, err, "parse_shortroll: ");
  }

  (*out)->nodeData.shortRoll.faces = faces;

  return RESULT_CODE_SUCCESS;
}

ResultCode parse_longroll(TokenIterator *tokeit, Tree **out, DErr **err)
{
  int die;
  ResultCode resultCode = parse_integer_raw(tokeit, &die, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    return derr_add_trace(resultCode, err, "parse_longroll die: ");
  }

  resultCode = consume_d(tokeit, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    return derr_add_trace(resultCode, err, "parse_longroll: ");
  }

  int faces;
  resultCode = parse_integer_raw(tokeit, &faces, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    return derr_add_trace(resultCode, err, "parse_longroll faces: ");
  }

  // FIXME h

  // FIXME l

  resultCode = tree_new(out, err, NODE_TYPE_LONGROLL);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    return derr_add_trace(resultCode, err, "parse_longroll: ");
  }

  (*out)->nodeData.longRoll.die = die;
  (*out)->nodeData.longRoll.faces = faces;

  return RESULT_CODE_SUCCESS;
}

ResultCode parse_integer(TokenIterator *tokeit, Tree **out, DErr **err)
{
  Tree *result;

  ResultCode resultCode = tree_new(&result, err, NODE_TYPE_INTEGER);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    return derr_add_trace(resultCode, err, "parse_roll: ");
  }

  resultCode =
      parse_integer_raw(tokeit, &result->nodeData.integer.integer, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    tree_free(&result);
    return derr_add_trace(resultCode, err, "parse_roll: ");
  }

  *out = result;

  return RESULT_CODE_SUCCESS;
}

ResultCode parse_roll(TokenIterator *tokeit, Tree **out, DErr **err)
{
  Token token;
  bool haveToken = tokeit_peek(tokeit, &token);
  if (haveToken && token.tokenType == TOKEN_TYPE_D)
  {
    return parse_shortroll(tokeit, out, err);
  }

  haveToken = tokeit_peek_next(tokeit, &token);
  if (haveToken && token.tokenType == TOKEN_TYPE_D)
  {
    return parse_longroll(tokeit, out, err);
  }

  return parse_integer(tokeit, out, err);
}

ResultCode parse_add(TokenIterator *tokeit, Tree **out, DErr **err);

ResultCode parse_atom(TokenIterator *tokeit, Tree **out, DErr **err)
{
  Token token;
  bool haveToken = tokeit_peek(tokeit, &token);
  if (haveToken && token.tokenType != TOKEN_TYPE_OPEN_PAREN)
  {
    return parse_roll(tokeit, out, err);
  }

  tokeit_next(tokeit, &token); // Consume the open paren
  ResultCode resultCode = parse_add(tokeit, out, err);
  tokeit_next(tokeit, &token); // Consume the close paren

  return resultCode;
}

ResultCode parse_mult(TokenIterator *tokeit, Tree **out, DErr **err)
{
  Tree *left = NULL;

  ResultCode resultCode = parse_atom(tokeit, &left, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    return derr_add_trace(resultCode, err, "parse_mult: parse_atom l: ");
  }

  Token token;
  bool haveToken = tokeit_peek(tokeit, &token);
  if (!haveToken)
  {
    *out = left;
    return RESULT_CODE_SUCCESS;
  }

  MathOperation op;
  switch (token.tokenType)
  {
  case TOKEN_TYPE_MULTIPLY:
    op = MATH_OP_MULTIPLY;
    break;
  case TOKEN_TYPE_DIVIDE:
    op = MATH_OP_DIVIDE;
    break;
  default:
    *out = left;
    return RESULT_CODE_SUCCESS;
  }

  tokeit_next(tokeit, &token); // consume the math op token

  Tree *right = NULL;

  resultCode = parse_atom(tokeit, &right, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    tree_free(&left);
    return derr_add_trace(resultCode, err, "parse_mult: parse_atom r: ");
  }

  resultCode = tree_new(out, err, NODE_TYPE_MATH);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    tree_free(&left);
    tree_free(&right);
    return derr_add_trace(resultCode, err, "parse_mult: ");
  }

  (*out)->nodeData.math.op = op;
  (*out)->nodeData.math.l = left;
  (*out)->nodeData.math.r = right;

  return RESULT_CODE_SUCCESS;
}

ResultCode parse_add(TokenIterator *tokeit, Tree **out, DErr **err)
{
  Tree *left = NULL;

  ResultCode resultCode = parse_atom(tokeit, &left, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    return derr_add_trace(resultCode, err, "parse_add: parse_atom l: ");
  }

  Token token;
  bool haveToken = tokeit_peek(tokeit, &token);
  if (!haveToken)
  {
    *out = left;
    return RESULT_CODE_SUCCESS;
  }

  MathOperation op;
  switch (token.tokenType)
  {
  case TOKEN_TYPE_PLUS:
    op = MATH_OP_ADD;
    break;
  case TOKEN_TYPE_MINUS:
    op = MATH_OP_SUBTRACT;
    break;
  default:
    *out = left;
    return RESULT_CODE_SUCCESS;
  }

  tokeit_next(tokeit, &token); // consume the math op token

  Tree *right = NULL;

  resultCode = parse_atom(tokeit, &right, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    tree_free(&left);
    return derr_add_trace(resultCode, err, "parse_add: parse_atom r: ");
  }

  resultCode = tree_new(out, err, NODE_TYPE_MATH);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    tree_free(&left);
    tree_free(&right);
    return derr_add_trace(resultCode, err, "parse_add: ");
  }

  (*out)->nodeData.math.op = op;
  (*out)->nodeData.math.l = left;
  (*out)->nodeData.math.r = right;

  return RESULT_CODE_SUCCESS;
}

ResultCode parse(TokenIterator *tokeit, Tree **out, DErr **err)
{
  ResultCode resultCode = validate_parenthesis(tokeit, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    return resultCode;
  }

  resultCode = parse_add(tokeit, out, err);
  if (resultCode != RESULT_CODE_SUCCESS)
  {
    return resultCode;
  }

  if (tokeit_has_next(tokeit))
  {
    tree_free(out);
    derr_set(
        err,
        "Did not consume all input tokens in parser.c, parse_expression",
        "Input is not valid dice algebra."
    );
    return RESULT_CODE_INTERNAL_ERROR;
  }

  return RESULT_CODE_SUCCESS;
}
