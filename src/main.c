#include <stdio.h>
#include <string.h>
#include "tokenize.h"
#include "global_error.h"

int main()
{
   printf("Please enter a roll: ");

   char userInput[50];

   fgets(userInput, sizeof(userInput), stdin);

   printf("You wrote: %s\n", userInput);

   TokenArray *ta = tokenize(userInput);
   if (is_there_an_error())
   {
      print_error();
      return 1;
   }

   for (unsigned int i = 0; i < ta->size; i++)
   {
      switch (ta->tokens[i].tokenType)
      {
      case TOKEN_TYPE_D:
         printf("d\n");
         break;
      case TOKEN_TYPE_H:
         printf("h\n");
         break;
      case TOKEN_TYPE_L:
         printf("l\n");
         break;
      case TOKEN_TYPE_PLUS:
         printf("+\n");
         break;
      case TOKEN_TYPE_MINUS:
         printf("-\n");
         break;
      case TOKEN_TYPE_MULTIPLY:
         printf("*\n");
         break;
      case TOKEN_TYPE_DIVIDE:
         printf("/\n");
         break;
      case TOKEN_TYPE_OPEN_PAREN:
         printf("(\n");
         break;
      case TOKEN_TYPE_CLOSE_PAREN:
         printf(")\n");
         break;
      case TOKEN_TYPE_INTEGER:
         printf("int: %d\n", ta->tokens[i].integerValue);
         break;
      }
   }

   return 0;
}
