#pragma once

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
  char *debugMessage;
  char *endUserMessage;
} DErr;

void derr_set(DErr **err, char *debugMessage, char *endUserMessage);

void derr_free(DErr *err);
