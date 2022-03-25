#include <stdio.h>
#include <stdint.h>
#include <winsock2.h>
#include "socket_util.h"

int open_socket(SOCKET* _socket, uint16_t port, uint32_t addr) {
    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s == INVALID_SOCKET) return -1;
    struct sockaddr_in socket_address = {0};
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(port);
    socket_address.sin_addr.S_un.S_addr = addr;
    int res = bind(s, (SOCKADDR*) &socket_address, sizeof(socket_address));
    if(res == SOCKET_ERROR) return -1;
    *_socket = s;
    return 0;
}
socket_destination make_destination(SOCKET s, uint16_t dest_port, uint32_t dest_addr) {
    socket_destination sd;
    sd.s = s;
    sd.dest = (struct sockaddr_in){0};
    sd.dest.sin_family = AF_INET;
    sd.dest.sin_port = htons(dest_port);
    sd.dest.sin_addr.S_un.S_addr = dest_addr;
    sd.dest_size = sizeof(sd.dest);
    return sd;
}

void print_socket_error(int code) {
    char *error_msg = NULL;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, code, 0, (LPTSTR)&error_msg, 0, 0);
    fprintf(stderr, "Error: %s\n", error_msg);
    LocalFree(error_msg);
}