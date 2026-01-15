#pragma once

#include <glib.h>

#define UNEXPECTED_ERR_MSG "An unexpected error has occurred."

// Result codes to return from function calls.
typedef enum
{
  RESULT_CODE_SUCCESS = 0,
  RESULT_CODE_INTERNAL_ERROR = 1,
} ResultCode;

// The "D" stands for the word "dice" in "dice algebra calculator error."
//
// This is used for passing useful error messages to be printed for end users.
typedef struct
{
  GString *debugMessage;
  GString *endUserMessage;
} DErr;

void derr_set_gstring(DErr **err, GString *message);

void derr_set(DErr **err, char *debugMessage, char *endUserMessage);

ResultCode
derr_add_trace(ResultCode resultCode, DErr **err, char *debugMessage);

void derr_free(DErr **err);
