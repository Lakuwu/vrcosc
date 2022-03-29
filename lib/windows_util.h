#ifndef WINDOWS_UTIL_HEADER
#define WINDOWS_UTIL_HEADER

#include <windows.h>

typedef struct {
    HWND hwnd;
    DWORD pid;
    const char *str;
} enum_windows_data;

DWORD find_pid(const char *name);
HWND find_hwnd(DWORD pid, const char *title);

void type(const char *text);
void press_key(int keycode);
void press_ctrl_and(int keycode);

void mouse_move(int dx, int dy);
void click_mouse_left();
void click_mouse_right();

#endif