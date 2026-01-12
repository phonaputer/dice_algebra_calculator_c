#include "global_error.h"
#include <execinfo.h>
#include <stdbool.h>
#include <string.h>

#define MAX_ERROR_MESSAGE_SIZE 256
#define MAX_STACK_FRAMES 50

static char errorMessage[MAX_ERROR_MESSAGE_SIZE] = "";
static bool isThereAnerror = false;

void set_error(char newErrorMessage[])
{
  isThereAnerror = true;

  strncpy(errorMessage, newErrorMessage, MAX_ERROR_MESSAGE_SIZE - 1);
  errorMessage[MAX_ERROR_MESSAGE_SIZE - 1] = '\0';
}

bool is_there_an_error(void) { return isThereAnerror; }

void clear_error(void) { isThereAnerror = false; }

void print_error(void)
{
  if (!isThereAnerror)
  {
    return;
  }

  printf("An error occurred: %s\n", errorMessage);
}
