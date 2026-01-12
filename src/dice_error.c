#include "dice_error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void derr_set(DErr **err, char *debugMessage, char *endUserMessage)
{
  if (err == NULL)
  {
    perror("Input err is unexpectedly null, dice_error.c, derr_set");
    return;
  }

  *err = (DErr *)malloc(sizeof(DErr));
  if (*err == NULL)
  {
    perror("Failed to malloc err, dice_error.c, derr_set");
    return;
  }

  size_t debugMsgLen = strlen(debugMessage) + 1;
  (*err)->debugMessage = (char *)malloc(debugMsgLen * sizeof(char));
  if ((*err)->debugMessage == NULL)
  {
    perror("Failed to malloc err->debugMessage, dice_error.c, derr_set");
    free(*err);
    return;
  }
  strncpy((*err)->debugMessage, debugMessage, debugMsgLen);

  size_t endUserMsgLen = strlen(endUserMessage) + 1;
  (*err)->endUserMessage = (char *)malloc(endUserMsgLen * sizeof(char));
  if ((*err)->endUserMessage == NULL)
  {
    perror("Failed to malloc err->endUserMessage, dice_error.c, derr_set");
    free((*err)->debugMessage);
    free(*err);
    return;
  }
  strncpy((*err)->endUserMessage, endUserMessage, endUserMsgLen);
}

void derr_free(DErr *err)
{
  if (err == NULL) return;

  if (err->debugMessage != NULL) free(err->debugMessage);
  if (err->endUserMessage != NULL) free(err->endUserMessage);

  free(err);
}
