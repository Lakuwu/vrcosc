#ifndef SOCKET_UTIL_HEADER
#define SOCKET_UTIL_HEADER

#include <winsock2.h>

typedef struct {
    SOCKET s;
    struct sockaddr_in dest;
    int dest_size;
} socket_destination;

int open_socket(SOCKET* _socket, uint16_t port, uint32_t addr);
socket_destination make_destination(SOCKET s, uint16_t dest_port, uint32_t dest_addr);

void print_socket_error(int code);
#endif