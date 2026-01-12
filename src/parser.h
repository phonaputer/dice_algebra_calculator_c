#pragma once

#include "lexer.h"

typedef enum
{
  RULE_ADD,
  RULE_MULTIPLY,
  RULE_ATOM,
  RULE_ROLL,
  RULE_LONGROLL,
  RULE_SHORTROLL,
  RULE_INTEGER
} RuleType;

struct _Tree;
typedef struct _Tree Tree;

union _RuleData;
typedef union _RuleData RuleData;

typedef struct
{
  Tree *l;
  Tree *r;
} AddMultRuleData;

typedef struct
{
  Tree *atom;
} AtomRuleData;

typedef struct
{
  Tree *roll;
} RollRuleData;

typedef struct
{
  Tree *die;
  Tree *faces;
  Tree *high;
  Tree *low;
} LongRollRuleData;

typedef struct
{
  Tree *integer;
} ShortRollRuleData;

typedef struct
{
  int i;
} IntegerRuleData;

struct
{
  RuleType ruleType;
  RuleData *ruleData;
} _Tree;

union _RuleData
{
  AddMultRuleData addMult;
  AtomRuleData atom;
  RollRuleData roll;
  LongRollRuleData longRoll;
  ShortRollRuleData shortRoll;
  IntegerRuleData integer;
};

ResultCode parse(TokenIterator *tokeit, Tree *out);

void free_tree(Tree *ast);
