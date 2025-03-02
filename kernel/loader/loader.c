
#include "loader.h"
#include "syscall.h"

#include <assert.h>
#include <string.h>

/**
 * @brief  Read the header of the BTBF binary
 * @param[in] header: Pointer to the header structure
 * @return BTBF_ERR_PARAM, BTBF_ERR_MAGIC or BTBF_OK
 */
BTBF_Status_t BTBF_ValidateHeader(BTBF_Header_t *header) {
   if (!header) {
      return BTBF_ERR_PARAM;
   }

   if (memcmp(&header->magic, BTBF_MAGIC, 4) != 0) {
      return BTBF_ERR_MAGIC;
   }

   return BTBF_OK;
}

/**
 * @brief  Get the total size of the memory to be allocated for the binary
 * @param[in] header: Pointer to the header structure
 * @return Total size in bytes
 */
size_t BTBF_GetAllocSize(const BTBF_Header_t *header) {
   assert(header);
   size_t ret = header->text_size + header->data_size + header->bss_size +
                header->got_size;
   return ret * 4;
}

/**
 * @brief  Initialize the application from the binary image
 * @param[in] header: Pointer to the header structure
 * @param[in] app: Pointer to the memory space for the app (must be already
 * @param[in] sys: Pointer to the system call table
 * copied and have exec permissions)
 * @return BTBF_OK or BTBF_ERR
 */
BTBF_Status_t BTBF_InitApp(const BTBF_Header_t *header, uint8_t *app,
                           const void *sys) {
   if (!header || !app) {
      return BTBF_ERR_PARAM;
   }

   uint32_t *sys_ptr = (uint32_t *)(app) + header->text_size;
   if (*sys_ptr != (uint32_t)DUMMY_SYS_ADDR) {
      return BTBF_ERR_SYS;
   }

   // patch the sys pointer
   *sys_ptr = (uint32_t)sys;


   // patch the GOT
   uint32_t *got = (uint32_t *)(app) + header->text_size + header->data_size;
   for (size_t i = 1; i < header->got_size - 2; i++) {
      got[i] += (uint32_t)app;
   }

   return BTBF_OK;
}
