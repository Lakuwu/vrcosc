#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "config.h"

config *config_new() {
    config *ret = malloc(sizeof(config));
    if(!ret) return NULL;
    ret->count = CONFIG_INITIAL_COUNT;
    ret->c = calloc(CONFIG_INITIAL_COUNT, sizeof(config_elem));
    if(!ret->c) {
        free(ret);
        return NULL;
    }
    return ret;
}

void config_free(config *cfg) {
    if(cfg) {
        free(cfg->c);
        free(cfg);
    }
}

static size_t config_find_key(config *cfg, const char *key) {
    bool match = false;
    size_t idx = 0;
    for(size_t i = 0; i < cfg->count; ++i) {
        config_elem *e = &cfg->c[i];
        if(!strncmp(key, e->key, CONFIG_KEY_LEN)) {
            match = true;
            idx = i;
            break;
        }
    }
    if(!match) return SIZE_MAX;
    return idx;
}

static size_t config_find_key_or_empty(config *cfg, const char *key) {
    bool match = false;
    bool found_empty = false;
    size_t idx = 0;
    size_t first_empty = 0;
    for(size_t i = 0; i < cfg->count; ++i) {
        config_elem *e = &cfg->c[i];
        if(!e->key[0] && !found_empty) {
            found_empty = true;
            first_empty = i;
        }
        if(!strncmp(key, e->key, CONFIG_KEY_LEN)) {
            match = true;
            idx = i;
            break;
        }
    }
    if(!match) {
        if(!found_empty) {
            size_t new_count = cfg->count * 2;
            config_elem *p = realloc(cfg->c, new_count * sizeof(config_elem));
            if(!p) return SIZE_MAX;
            cfg->c = p;
            memset(cfg->c + cfg->count, 0, cfg->count * sizeof(config_elem));
            first_empty = cfg->count;
            cfg->count = new_count;
        }
        idx = first_empty;
        strncpy(cfg->c[idx].key, key, CONFIG_KEY_LEN);
        cfg->c[idx].key[CONFIG_KEY_LEN - 1] = '\0';
    }
    return idx;
}

int config_get_int(config *cfg, const char *key, int32_t *val) {
    if(!cfg) return -1;
    size_t idx = config_find_key(cfg, key);
    if(idx == SIZE_MAX) return -1;
    config_elem *elem = &cfg->c[idx];
    if(elem->type != CONFIG_INT) return -1;
    *val = elem->i;
    return 0;
}

int config_set_int(config *cfg, const char *key, int32_t val) {
    if(!cfg) return -1;
    size_t idx = config_find_key_or_empty(cfg, key);
    if(idx == SIZE_MAX) return -1;
    config_elem *elem = &cfg->c[idx];
    elem->type = CONFIG_INT;
    elem->i = val;
    return 0;
}

int config_get_string(config *cfg, const char *key, const char **str) {
    if(!cfg) return -1;
    size_t idx = config_find_key(cfg, key);
    if(idx == SIZE_MAX) return -1;
    config_elem *elem = &cfg->c[idx];
    if(elem->type != CONFIG_STRING) return -1;
    *str = elem->s;
    return 0;
}

int config_set_string(config *cfg, const char *key, const char *str) {
    if(!cfg) return -1;
    size_t idx = config_find_key_or_empty(cfg, key);
    if(idx == SIZE_MAX) return -1;
    config_elem *elem = &cfg->c[idx];
    elem->type = CONFIG_STRING;
    strncpy(elem->s, str, CONFIG_VALUE_LEN);
    elem->s[CONFIG_VALUE_LEN - 1] = '\0';
    return 0;
}

static inline void trim_end(char *str, char ch) {
    for(char *c = str + strlen(str) - 1; c > str && *c == ch; --c) *c = '\0';
}

int config_load(config *cfg, const char *filename) {
    if(!cfg) return -1;
    FILE *fp = fopen(filename, "r");
    if(!fp) return -1;
    char buf[CONFIG_LOAD_BUF_LEN];
    while(!feof(fp)) {
        memset(buf, 0, CONFIG_LOAD_BUF_LEN);
        char *p = fgets(buf, CONFIG_LOAD_BUF_LEN, fp);
        if(!p) break;
        strtok(buf, "\r\n");
        char *key = strtok(buf, "=");
        if(key[0] == ' ') key += strspn(key, " ");
        trim_end(key, ' ');
    
        char *val = strtok(NULL, "\r\n");
        if(!val) {
            // no value
            continue;
        }
        if(val[0] == ' ') val += strspn(val, " ");
        trim_end(val, ' ');
        config_type type = CONFIG_NULL;
        if(val[0] == '"') {
            val = strtok(val, "\"");
            type = CONFIG_STRING;
        } else {
            type = CONFIG_INT;
        }
        size_t idx = config_find_key_or_empty(cfg, key);
        if(idx == SIZE_MAX) {
            // out of memory
            continue;
        }
        config_elem *elem = &cfg->c[idx];
        if(elem->type != CONFIG_NULL && elem->type != type) continue;
        elem->type = type;
        switch(type) {
            case CONFIG_INT: {
                char *end = NULL;
                int32_t i = strtol(val, &end, 10);
                if(val == end) {
                    // invalid value
                    continue;
                }
                elem->i = i;
                break;
            }
            case CONFIG_STRING: {
                strncpy(elem->s, val, CONFIG_VALUE_LEN);
                elem->s[CONFIG_VALUE_LEN - 1] = '\0';
                break;
            }
            default: {
                // unknown type
            }
        }
    }
    fclose(fp);
    return 0;
}

int config_save(config *cfg, const char *filename) {
    if(!cfg) return -1;
    FILE *fp = fopen(filename, "w");
    if(!fp) return -1;
    for(size_t i = 0; i < cfg->count; ++i) {
        config_elem *e = &cfg->c[i];
        switch(e->type) {
            case CONFIG_INT: {
                fprintf(fp, "%s=%d\n", e->key, e->i);
                break;
            }
            case CONFIG_STRING: {
                fprintf(fp, "%s=\"%s\"\n", e->key, e->s);
                break;
            }
            case CONFIG_NULL: {
                // do nothing
                break;
            }
            default: {
                // unknown type
            }
        }
    }
    fclose(fp);
    return 0;
}

void config_dump(config *cfg) {
    for(size_t i = 0; i < cfg->count; ++i) {
        config_elem *e = &cfg->c[i];
        printf("[% 2lld] ", i);
        switch(e->type) {
            case CONFIG_INT: printf("int\t%s: %d\n", e->key, e->i); break;
            case CONFIG_STRING: printf("string\t%s: \"%s\"\n", e->key, e->s); break;
            case CONFIG_NULL: printf("null\n"); break;
            default: printf("unknown\n");
        }
    }
}