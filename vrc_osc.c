#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "vrc_osc.h"
#include <windows.h>


int osc_send_float(socket_destination sd, const char *path, float f) {
    int path_strlen = strlen(path);
    int path_len = ALIGN4(path_strlen + 1);
    uint8_t *buf = calloc(path_len + 8, 1);
    uint8_t *p = buf;
    memcpy(p, path, path_strlen);
    p += path_len;
    char type[3] = ",f";
    int type_strlen = 2;
    int type_len = 4;
    memcpy(p, type, type_strlen);
    p += type_len;
    uint32_t val = _byteswap_ulong(*(uint32_t*)&f);
    memcpy(p, &val, 4);
    int res  = sendto(sd.s, (char*)buf, path_len + 8, 0, (SOCKADDR*)&sd.dest, sd.dest_size);
    free(buf);
    return res;
}

int osc_send_int(socket_destination sd, const char *path, int i) {
    int path_strlen = strlen(path);
    int path_len = ALIGN4(path_strlen + 1);
    uint8_t *buf = calloc(path_len + 8, 1);
    uint8_t *p = buf;
    memcpy(p, path, path_strlen);
    p += path_len;
    char type[3] = ",i";
    int type_strlen = 2;
    int type_len = 4;
    memcpy(p, type, type_strlen);
    p += type_len;
    uint32_t val = _byteswap_ulong(i);
    memcpy(p, &val, 4);
    int res  = sendto(sd.s, (char*)buf, path_len + 8, 0, (SOCKADDR*)&sd.dest, sd.dest_size);
    free(buf);
    return res;
}

int osc_send_str(socket_destination sd, const char *path, const char *str) {
    int path_strlen = strlen(path);
    int path_len = ALIGN4(path_strlen + 1);
    int str_strlen = strlen(str);
    int str_len = ALIGN4(str_strlen + 1);
    size_t msg_len = path_len + 4 + str_len;
    uint8_t *buf = calloc(msg_len, 1);
    uint8_t *p = buf;
    memcpy(p, path, path_strlen);
    p += path_len;
    char type[3] = ",s";
    int type_strlen = 2;
    int type_len = 4;
    memcpy(p, type, type_strlen);
    p += type_len;
    memcpy(p, str, str_strlen);
    int res  = sendto(sd.s, (char*)buf, msg_len, 0, (SOCKADDR*)&sd.dest, sd.dest_size);
    free(buf);
    return res;
}

void print_osc_data(void *data) {
    char *path = data;
    int path_len = ALIGN4(strlen(path) + 1);
    char *type = (path + path_len);
    int type_strlen = strlen(type);
    int type_len = ALIGN4(type_strlen + 1);
    uint8_t *p = (uint8_t*)(type + type_len);
    printf("%s ", path);
    for(int i = 1; i < type_strlen; ++i) {
        uint32_t buf = _byteswap_ulong(*(uint32_t*)p);
        void *pbuf = &buf;
        printf("%c ", type[i]);
        switch(type[i]) {
            case 'i': {
                printf("%d\n", *(int*)pbuf);
                p +=4 ;
                break;
            }
            case 'f': {
                printf("%f\n", *(float*)pbuf);
                p += 4;
                break;
            }
            case 's': {
                int len = ALIGN4(strlen((char*)p) + 1);
                printf("%s\n", p);
                p += len;
                break;
            }
            case 'F': {
                printf("false\n");
                break;
            }
            case 'T': {
                printf("true\n");
                break;
            }
            default: {
                printf("unknown\n");
                break;
            }
        }
    }
}

void osc_parse_message(void *data, osc_message *msg) {
    memset(msg, 0 ,sizeof(osc_message));
    char *path = data;
    int path_len = ALIGN4(strlen(path) + 1);
    strncpy(msg->path, path, OSC_PATH_LEN - 1);
    
    char *type = (path + path_len);
    int type_strlen = strlen(type);
    int type_len = ALIGN4(type_strlen + 1);
    
    void *blob = (type + type_len);
    uint32_t swap = _byteswap_ulong(*(uint32_t*)blob);
    
    // VRChat supports a single item per message
    switch(type[1]) {
        case 'i': {
            msg->type = OSC_INT;
            msg->i = *(int32_t*)&swap;
            break;
        }
        case 'f': {
            msg->type = OSC_FLOAT;
            msg->f = *(float*)&swap;
            break;
        }
        case 's': {
            msg->type = OSC_STRING;
            strncpy(msg->s, blob, OSC_STRING_LEN - 1);
            break;
        }
        case 'T': {
            msg->type = OSC_TRUE;
            break;
        }
        case 'F': {
            msg->type = OSC_FALSE;
            break;
        }
    }
}

void print_osc_message(osc_message *msg) {
    printf("%s ", msg->path);
    switch(msg->type) {
        case OSC_INT:    printf("i %d\n", msg->i); break;
        case OSC_FLOAT:  printf("f %f\n", msg->f); break;
        case OSC_STRING: printf("s %s\n", msg->s); break;
        case OSC_TRUE:   printf("T\n"); break;
        case OSC_FALSE:  printf("F\n"); break;
        default: printf("unknown type\n"); break;
    }
}