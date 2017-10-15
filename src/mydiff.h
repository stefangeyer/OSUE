#ifndef MYDIFF_H
#define MYDIFF_H
#define LINE_MAX 100
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
// Using the C99 standard on windows seems to prevent the POSIX macros from being defined (warnings) -> Include manually
#include <getopt.h>
#include <assert.h>

void difference(FILE *file1, FILE *file2, bool ignore_case);

#endif // MYDIFF_H
