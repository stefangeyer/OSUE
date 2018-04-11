#include "common.h"

int max(int a, int b) {
    return a > b ? a : b;
}

int min(int a, int b) {
    return a < b ? a : b;
}

uint8_t calculate_parity(int x, int limit) {
    uint8_t parity = 0;

    // xor every bit for 0 to limit (incl.)
    for (int i = 0; i <= limit; i++) {
        parity ^= x & 1;
        x >>= 1;
    }

    return parity;
}