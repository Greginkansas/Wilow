struct willow_config {
    char *wis_server_url;
    char *wis_tts_url;
};

extern struct willow_config wc;

void config_parse(void);