#define SYSCALL_IMPL
#include "syscall.h"

int x = 12;
int *y = &x; // This won't work for now.
static int *z = &x; // But this will.

int calc(int a, int b) { return a + b; }

int main(void) {
   for (size_t i = 0; i < x; i++) {
      sys->log("Hello, world! %d\n", calc(i, *y + *z));
      sys->delay_Ms(1000);
   }
}

