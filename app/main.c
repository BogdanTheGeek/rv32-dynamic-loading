#define SYSCALL_IMPL
#include "syscall.h"

#if 1
int x = 12;
int *y = &x; // This won't work for now.
static int *z = &x; // But this will.
#else 
static int x = 12;
static int *y = &x;
static int *z = &x;
#endif

int calc(int a, int b) { return a + b; }

int val = 0;

int main(void) {
   val = (int)calc;
   for (int i = 0; i < x; i++) {
      sys->log("Hello, world! %d\n", calc(i, *y + *z));
      sys->delay_Ms(1000);
   }
   sys->log("Goodbye, world! %d\n", val);
}

