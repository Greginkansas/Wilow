enum speech_rec_mode {
    WILLOW_USE_WIS = 0,
    WILLOW_USE_MULTINET,
};

struct willow_config {
    enum speech_rec_mode speech_rec_mode;
    char *wis_server_url;
    char *wis_tts_url;
};

extern struct willow_config wc;

void config_parse(void);
esp_err_t init_was(void);
