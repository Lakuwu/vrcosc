#ifndef VRC_OSC_HEADER
#define VRC_OSC_HEADER

#include <stdint.h>

#include "socket_util.h"
#include "windows_util.h"

#define OSC_PATH_LEN 128
#define OSC_STRING_LEN 128

#define ALIGN4(addr) (((addr) + 3) & (~3llu))

typedef enum {
    OSC_NULL = 0,
    OSC_INT,
    OSC_FLOAT,
    OSC_STRING,
    OSC_TRUE,
    OSC_FALSE
} osc_type;

typedef struct {
    osc_type type;
    char path[OSC_PATH_LEN];
    union {
        int32_t i;
        float f;
        char s[OSC_STRING_LEN];
    };
} osc_message;

int osc_send_int(socket_destination sd, const char *path, int i);
int osc_send_float(socket_destination sd, const char *path, float f);
int osc_send_str(socket_destination sd, const char *path, const char *str);

int osc_parse_message(void *data, osc_message *msg);
void print_osc_message(osc_message *msg);
void print_osc_data(void *data);
#endif