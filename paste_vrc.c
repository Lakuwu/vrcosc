#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "vrc_osc.h"
#include "config.h"

#define RECEIVE_BUFFER_LENGTH 512

config *cfg = NULL;

void vrchat_paste() {
    HWND curr_window = GetForegroundWindow();
    DWORD pid = find_pid("VRChat.exe");
    if(!pid) return;
    HWND hwnd = find_hwnd(pid, "UnityWndClass");
    if(!hwnd) return;
    SetForegroundWindow(hwnd);
    press_ctrl_and('V');
    SetForegroundWindow(curr_window);
    return;
}

int osc_main(SOCKET s) {
    const char *parameter = NULL;
    config_get_string(cfg, "paste_parameter", &parameter);
    
    char recv[RECEIVE_BUFFER_LENGTH];
    osc_message msg = {0};
    while(1) {
        memset(recv, 0, RECEIVE_BUFFER_LENGTH);
        if(recvfrom(s, recv, RECEIVE_BUFFER_LENGTH-1, 0, NULL, NULL) == SOCKET_ERROR) {
            return WSAGetLastError();
        }
        osc_parse_message(recv, &msg);
        if(msg.type == OSC_TRUE && !strncmp(msg.path, parameter, OSC_STRING_LEN - 1)) {
            vrchat_paste();
        }
    }
    return 0;
}

_Noreturn void exit_error(const char *message, int code) {
    fprintf(stderr, "%s\n", message);
    print_socket_error(code);
    WSACleanup();
    config_free(cfg);
    puts("Press ENTER to exit.");
    getchar();
    exit(-1);
}

int main() {
    SetConsoleTitle("paste_vrc - https://github.com/Lakuwu/vrcosc");
    const char cfg_filename[] = "paste_vrc.ini";
    cfg = config_new();
    config_set_int(cfg, "listening_port", 9001);
    config_set_string(cfg, "paste_parameter", "/avatar/parameters/paste");
    bool have_config = !config_load(cfg, cfg_filename);
    int port = 0;
    config_get_int(cfg, "listening_port", &port);
    port = (uint16_t)port;
    config_set_int(cfg, "listening_port", port);
    if(have_config) config_save(cfg, cfg_filename);
    
    WSADATA wsa;
    int res = WSAStartup(MAKEWORD(2,2), &wsa);
    if(res) {
        exit_error("Failed to initialize Winsock.", res);
    }
    
    SOCKET s = INVALID_SOCKET;
    config_get_int(cfg, "listening_port", &port);
    if(open_socket(&s, port, htonl(INADDR_ANY))) {
        exit_error("Failed to open socket.", WSAGetLastError());
    }
    
    printf("Listening for OSC messages on port %d.\n", port);
    res = osc_main(s);
    if(res) {
        exit_error("Error while trying to receive data.", res);
    }
    
    WSACleanup();
    config_free(cfg);
    
    return 0;
}