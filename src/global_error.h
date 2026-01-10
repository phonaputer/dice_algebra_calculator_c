#pragma once

#include <stdbool.h>
#include <stdio.h>

void set_error(char message[]);

bool is_there_an_error();

void clear_error();

void print_error(int outputFd);