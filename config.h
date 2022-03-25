#ifndef CONFIG_HEADER
#define CONFIG_HEADER

#include <stdint.h>

#define CONFIG_KEY_LEN 128
#define CONFIG_VALUE_LEN 128
#define CONFIG_INITIAL_COUNT 1
#define CONFIG_LOAD_BUF_LEN 256

typedef enum {
    CONFIG_NULL = 0,
    CONFIG_INT,
    CONFIG_STRING
} config_type;

typedef struct {
    config_type type;
    char key[CONFIG_KEY_LEN];
    union {
        int32_t i;
        char s[CONFIG_VALUE_LEN];
    };
} config_elem;

typedef struct {
    config_elem *c;
    size_t count;
} config;

config *config_new();
void config_free(config *cfg);

int config_get_int(config *cfg, const char *key, int32_t *val);
int config_set_int(config *cfg, const char *key, int32_t val);
int config_get_string(config *cfg, const char *key, const char **str);
int config_set_string(config *cfg, const char *key, const char *str);

int config_load(config *cfg, const char *filename);
int config_save(config *cfg, const char *filename);

void config_dump(config *cfg);
#endif