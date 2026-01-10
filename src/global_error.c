#include <stdbool.h>
#include <string.h>
#include <execinfo.h>
#include "global_error.h"

#define MAX_ERROR_MESSAGE_SIZE 256
#define MAX_STACK_FRAMES 50

static char errorMessage[MAX_ERROR_MESSAGE_SIZE] = "";
static bool isThereAnerror = false;

void set_error(char newErrorMessage[])
{
    isThereAnerror = true;

    strncpy(errorMessage, newErrorMessage, MAX_ERROR_MESSAGE_SIZE - 1);
    errorMessage[MAX_ERROR_MESSAGE_SIZE - 1] = '\0';

    printf("%s\n", errorMessage);
}

bool is_there_an_error()
{
    return isThereAnerror;
}

void clear_error()
{
    isThereAnerror = false;
}

void print_error()
{
    if (!isThereAnerror)
    {
        return;
    }

    printf("An error occurred: %s\n", errorMessage);
}