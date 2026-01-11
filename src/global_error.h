#pragma once

#include <stdbool.h>
#include <stdio.h>

// TODO: this feels super weird and probably should be changed to a pattern like
// returning an error code or error struct from each function which could error.

void set_error(char message[]);

bool is_there_an_error();

void clear_error();

void print_error();