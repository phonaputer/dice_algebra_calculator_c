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

void prepend(char *s, const char *t)
{
  size_t len = strlen(t);
  memmove(s + len, s, strlen(s) + 1);
  memcpy(s, t, len);
}

ResultCode derr_add_trace(ResultCode code, DErr **err, char *debugMessage)
{
  if (err == NULL) return code;
  if ((*err) == NULL) return code;
  if ((*err)->debugMessage == NULL) return code;

  size_t newSize = strlen((*err)->debugMessage) + strlen(debugMessage) + 1;

  (*err)->debugMessage = realloc((*err)->debugMessage, newSize);
  if ((*err)->debugMessage == NULL)
  {
    perror("Failed to realloc err->debugMessage, dice_error.c, derr_add_trace");
    return code;
  }

  prepend((*err)->debugMessage, debugMessage);

  return code;
}

void derr_free(DErr **errP)
{
  if (errP == NULL) return;

  DErr *err = *errP;

  if (err->debugMessage != NULL)
  {
    free(err->debugMessage);
    err->debugMessage = NULL;
  }

  if (err->endUserMessage != NULL)
  {
    free(err->endUserMessage);
    err->endUserMessage = NULL;
  }

  free(err);
  *errP = NULL;
}
