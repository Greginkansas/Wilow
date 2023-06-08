#include <stdio.h>
#include <sys/stat.h>

#include "esp_log.h"
#include "esp_spiffs.h"

#define CONFIG_PATH "/spiffs/user/config/willow.json"

static const char *TAG = "WILLOW/CONFIG";

void config_print(void)
{
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    ESP_LOGI(TAG, "opening %s", CONFIG_PATH);
    FILE *f = fopen(CONFIG_PATH, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "failed to open %s", CONFIG_PATH);
        goto close;
    }
    struct stat fs;
    if (stat(CONFIG_PATH, &fs)) {
        ESP_LOGE(TAG, "failed to get file status");
        goto close;
    }
    ESP_LOGI(TAG, "config file size: %ld", fs.st_size);

    char line[4096];
    ESP_LOGI(TAG, "config file content:");
    while (fgets(line, sizeof(line), f) != NULL) {
        printf("%s\n", line);
    };
close:
    fclose(f);
}