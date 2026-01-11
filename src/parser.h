#pragma once

#include "lexer.h";

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

typedef struct
{
  AST *l;
  AST *r;
} AddMultRuleData;

typedef struct
{
  AST *atom;
} AtomRuleData;

typedef struct
{
  AST *roll;
} RollRuleData;

typedef struct
{
  AST *die;
  AST *faces;
  AST *high;
  AST *low;
} LongRollRuleData;

typedef struct
{
  AST *integer;
} ShortRollRuleData;

typedef struct
{
  int i;
} IntegerRuleData;

union RuleData
{
  data_type member1;
  data_type member2;
  // ...
};

typedef struct
{
  RuleType ruleType;
  Token *token;
  int integerValue;
} AST;
