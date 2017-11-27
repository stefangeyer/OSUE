#ifndef CHILD_H
#define CHILD_H

#include <stdbool.h>

void child_process(void (*parent)(int[]), void (*child)(int[]));

#endif //UE2_CHILD_H
