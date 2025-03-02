#pragma once

#include <stddef.h>

#define DUMMY_SYS_ADDR 0xA5A5A5A5

typedef struct {
  void (*delay_Ms)(size_t ms);
  void (*log)(const char *fmt, ...);
} sys_t;

#ifdef KERNEL
extern sys_t sys;
#else
#ifdef SYSCALL_IMPL
const sys_t *sys __attribute__((unused, section(".sys_struct"))) =
    (sys_t *)(DUMMY_SYS_ADDR);
#else
extern const sys_t *sys;
#endif
#endif
