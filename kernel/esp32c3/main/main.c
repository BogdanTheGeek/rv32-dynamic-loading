#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "loader.h"
#include "syscall.h"

static const char *TAG = "kernel";

typedef int (*main_t)(void);

void task_wrapper(void *app) {
   const main_t f = (main_t)app;
   int ret = f();
   ESP_LOGI(TAG, "App returned %d", ret);
   free(app);
   vTaskDelete(NULL);
}

static void load_app(const char *filename) {
   ESP_LOGI(TAG, "Reading %s", filename);

   // Open for reading hello.txt
   FILE *f = fopen(filename, "rb");
   if (f == NULL) {
      ESP_LOGE(TAG, "Failed to open %s", filename);
      return;
   }

   BTBF_Header_t btbf;
   const size_t ret = fread(&btbf, 1, sizeof(btbf), f);

   if (ret != sizeof(btbf)) {
      ESP_LOGE(TAG, "Failed to read BTBF header");
      fclose(f);
      return;
   }

   if (BTBF_OK != BTBF_ValidateHeader(&btbf)) {
      ESP_LOGE(TAG, "Invalid BTBF header");
      fclose(f);
      return;
   }

   ESP_LOGI(TAG, "BTBF header OK");

   const size_t app_size = BTBF_GetAllocSize(&btbf);

   ESP_LOGI(TAG, "App size: %d", app_size);

   uint8_t *buf = heap_caps_malloc(app_size, MALLOC_CAP_RTCRAM);
   assert(buf);

   const size_t read = fread(buf, 1, app_size, f);
   if (read != app_size) {
      ESP_LOGE(TAG, "Failed to read %s", filename);
      fclose(f);
      return;
   }
   fclose(f);

   if (BTBF_OK != BTBF_InitApp(&btbf, buf, &sys)) {
      ESP_LOGE(TAG, "Could not initialize app");
      return;
   }

   ESP_LOGI(TAG, "Starting app (%s) at %p", btbf.name, buf);
   // Create a task to run the app
   (void)xTaskCreate(task_wrapper, btbf.name, 4096, buf, 1, NULL);
}

void app_main(void) {
   ESP_LOGI(TAG, "Initializing SPIFFS");

   esp_vfs_spiffs_conf_t conf = {.base_path = "/spiffs",
                                 .partition_label = NULL,
                                 .max_files = 5,
                                 .format_if_mount_failed = false};

   // Use settings defined above to initialize and mount SPIFFS filesystem.
   // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
   esp_err_t ret = esp_vfs_spiffs_register(&conf);

   if (ret != ESP_OK) {
      if (ret == ESP_FAIL) {
         ESP_LOGE(TAG, "Failed to mount or format filesystem");
      } else if (ret == ESP_ERR_NOT_FOUND) {
         ESP_LOGE(TAG, "Failed to find SPIFFS partition");
      } else {
         ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)",
                  esp_err_to_name(ret));
      }
      return;
   }

   size_t total = 0, used = 0;
   ret = esp_spiffs_info(NULL, &total, &used);
   if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)",
               esp_err_to_name(ret));
   } else {
      ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
   }

   load_app("/spiffs/app.btbf");
   vTaskDelay(pdMS_TO_TICKS(500));
   load_app("/spiffs/app.btbf");
   vTaskDelay(pdMS_TO_TICKS(1000));

   // All done, unmount partition and disable SPIFFS
   esp_vfs_spiffs_unregister(NULL);
   ESP_LOGI(TAG, "SPIFFS unmounted");
}
