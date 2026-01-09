#include <stdio.h>
#include <string.h>

int main()
{
   printf("Please enter a roll: ");

   char userInput[50];

   fgets(userInput, sizeof(userInput), stdin);

   printf("You wrote: %s\n", userInput);

   return 0;
}
