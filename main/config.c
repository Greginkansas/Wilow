#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "cJSON.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_transport_ws.h"
#include "esp_websocket_client.h"

#include "config.h"

#define CONFIG_PATH "/spiffs/user/config/willow.json"

static const char *TAG = "WILLOW/CONFIG";
static esp_websocket_client_handle_t hdl_wc = NULL;

struct willow_config wc;

static char *config_read(void)
{
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
    char *config = NULL;

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

    config = calloc(sizeof(char), fs.st_size + 1);
    size_t rlen = fread(config, 1, fs.st_size, f);
    ESP_LOGI(TAG, "fread: %d", rlen);
    config[fs.st_size] = '\0';
    ESP_LOGI(TAG, "config file content: %s", config);
close:
    fclose(f);

    return config;
}

void config_parse(void)
{
    char *config = config_read();
    char *json = NULL;
    cJSON *cjson = cJSON_Parse(config);
    if (cjson == NULL) {
        const char *eptr = cJSON_GetErrorPtr();
        if (eptr != NULL) {
            ESP_LOGE(TAG, "error parsing config file: %s\n", eptr);
            goto cleanup;
        }
    }

    json = cJSON_Print(cjson);
    ESP_LOGI(TAG, "parsed config file:");
    printf("%s\n", json);

    cJSON *wis_server_url = cJSON_GetObjectItemCaseSensitive(cjson, "WILLOW_WIS_SERVER_URL");
    if (cJSON_IsString(wis_server_url) && wis_server_url->valuestring != NULL) {
        ESP_LOGI(TAG, "parsed WIS server URL from config");
        wc.wis_server_url = strdup(wis_server_url->valuestring);
    }

    cJSON *wis_tts_url = cJSON_GetObjectItemCaseSensitive(cjson, "WILLOW_WIS_TTS_URL");
    if (cJSON_IsString(wis_tts_url) && wis_tts_url->valuestring != NULL) {
        ESP_LOGI(TAG, "parsed WIS TTS server URL from config");
        wc.wis_tts_url = strdup(wis_tts_url->valuestring);
    }

cleanup:
    cJSON_Delete(cjson);
    free(config);
}

static void cb_ws_event(const void *arg_evh, const esp_event_base_t *base_ev, const int32_t id_ev, const void *ev_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)ev_data;
    // components/esp_websocket_client/include/esp_websocket_client.h - enum esp_websocket_event_id_t
    switch (id_ev) {
        case WEBSOCKET_EVENT_CONNECTED:
            ESP_LOGI(TAG, "WebSocket connected");
            break;
        case WEBSOCKET_EVENT_DATA:
            ESP_LOGD(TAG, "WebSocket data received");
            if (data->op_code == WS_TRANSPORT_OPCODES_TEXT) {
                char *resp = strndup((char *)data->data_ptr, data->data_len);
                ESP_LOGI(TAG, "received text data on WebSocket: %s", resp);
                free(resp);
            }
            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "WebSocket disconnected");
            break;
        case WEBSOCKET_EVENT_CLOSED:
            ESP_LOGI(TAG, "WebSocket closed");
            break;
        default:
            ESP_LOGI(TAG, "unhandled WebSocket event - ID: %d", id_ev);
            break;
    }
}

esp_err_t init_was(void)
{
    const esp_websocket_client_config_t cfg_wc = {
        .uri = CONFIG_WILLOW_WAS_URL,
    };
    esp_err_t err = ESP_OK;

    hdl_wc = esp_websocket_client_init(&cfg_wc);

    esp_websocket_register_events(hdl_wc, WEBSOCKET_EVENT_ANY, (esp_event_handler_t)cb_ws_event, NULL);

    err = esp_websocket_client_start(hdl_wc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to start WebSocket client: %s", esp_err_to_name(err));
    }
    return err;
}