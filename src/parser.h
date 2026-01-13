#pragma once

#include "dice_error.h"
#include "lexer.h"

typedef enum
{
  NODE_TYPE_MATH,
  NODE_TYPE_LONGROLL,
  NODE_TYPE_SHORTROLL,
  NODE_TYPE_INTEGER
} NodeType;

typedef enum
{
  MATH_OP_ADD,
  MATH_OP_SUBTRACT,
  MATH_OP_MULTIPLY,
  MATH_OP_DIVIDE
} MathOperation;

struct _Tree;
typedef struct _Tree Tree;

typedef struct
{
  Tree *l;
  Tree *r;
  MathOperation op;
} MathNodeData;

typedef struct
{
  int die;
  int faces;
  int high;
  int low;
} LongRollNodeData;

typedef struct
{
  int faces;
} ShortRollNodeData;

typedef struct
{
  int integer;
} IntegerNodeData;

typedef union
{
  MathNodeData math;
  LongRollNodeData longRoll;
  ShortRollNodeData shortRoll;
  IntegerNodeData integer;
} NodeData;

struct _Tree
{
  NodeType nodeType;
  NodeData nodeData;
};

void tree_free(Tree **tree);

ResultCode parse(TokenIterator *tokeit, Tree **out, DErr **err);
