#include "dice_error.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void derr_set_gstring(DErr **err, GString *message)
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

  (*err)->debugMessage = g_string_new(message->str);
  (*err)->endUserMessage = message;
}

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

  GString *debugGString = g_string_new(debugMessage);
  GString *endUserGString = g_string_new(endUserMessage);

  (*err)->debugMessage = debugGString;
  (*err)->endUserMessage = endUserGString;
}

ResultCode derr_add_trace(ResultCode code, DErr **err, char *debugMessage)
{
  if (err == NULL) return code;
  if ((*err) == NULL) return code;
  if ((*err)->debugMessage == NULL) return code;

  g_string_prepend((*err)->debugMessage, debugMessage);

  return code;
}

void derr_free(DErr **errP)
{
  if (errP == NULL) return;

  DErr *err = *errP;

  g_string_free(err->debugMessage, TRUE);
  g_string_free(err->endUserMessage, TRUE);

  free(err);
  *errP = NULL;
}
