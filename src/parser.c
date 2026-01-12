#include "parser.h"

Tree *parse(TokenIterator *tokeit) { return NULL; }

void free_tree(Tree *tree)
{
  if (tree == NULL) return;

  if (tree->ruleData != NULL) free(tree->ruleData);

  free(tree);
}