
#pragma once

#include <stddef.h>
#include <stdint.h>

#define BTBF_MAGIC "BTBF"

typedef struct {
  char magic[4]; // always "BTBF"
  char name[16]; // null terminalted
  uint8_t major;
  uint8_t minor;
  uint16_t text_size; // in words
  uint16_t data_size; // in words
  uint16_t bss_size;  // in words
  uint16_t got_size;  // in words
  uint8_t data[];
} BTBF_Header_t;

typedef enum {
  BTBF_OK = 0,
  BTBF_ERR_PARAM = 1,
  BTBF_ERR_MAGIC = 2,
  BTBF_ERR_SYS = 3,
} BTBF_Status_t;

BTBF_Status_t BTBF_ValidateHeader(BTBF_Header_t *header);
size_t BTBF_GetAllocSize(const BTBF_Header_t *header);
BTBF_Status_t BTBF_InitApp(const BTBF_Header_t *header, uint8_t *app,
                           const void *sys);
