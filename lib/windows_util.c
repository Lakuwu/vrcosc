#include <stdio.h>

#include <windows.h>
#include <tlhelp32.h>

#include "windows_util.h"

void print_hwnd_title(HWND hwnd) {
    char title[256];
    GetClassName(hwnd,title,256);
    printf("%s\n", title);
}

DWORD find_pid(const char *name) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    
    if(!Process32First(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return 0;
    }
    
    do {
        if(!strcmp(pe32.szExeFile, name)) {
            CloseHandle(hSnapshot);
            return pe32.th32ProcessID;
        }
    } while(Process32Next(hSnapshot, &pe32));
    CloseHandle(hSnapshot);
    return 0;
}

BOOL CALLBACK FindTitleProc(HWND hwnd, LPARAM lParam) {
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    enum_windows_data *e = (void*)lParam;
    if(pid == e->pid) {
        char title[256];
        GetClassName(hwnd, title, 256);
        if(!strcmp(e->str, title)) {
            e->hwnd = hwnd;
            return FALSE;
        }
    }
    return TRUE;
}

HWND find_hwnd(DWORD pid, const char *title) {
    enum_windows_data e;
    e.str = title;
    e.pid = pid;
    if(EnumWindows(FindTitleProc, (LPARAM) &e)) {
        return 0;
    }
    return e.hwnd;
}

BOOL CALLBACK enumhwnd(HWND hwnd, LPARAM lParam) {
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    if(pid == lParam) print_hwnd_title(hwnd);
    return TRUE;
}

void list_hwnd(DWORD pid) {
    EnumWindows(enumhwnd, pid);
}

void print_pe32(PROCESSENTRY32 *p) {
    printf("%32s\tPID: %5ld\tParent PID: %5ld\n", p->szExeFile, p->th32ProcessID, p->th32ParentProcessID);
}

void list_processes() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    
    if(!Process32First(hSnapshot, &pe32)) {
        printf("fug\n");
        CloseHandle(hSnapshot);
        return;
    }
    
    do {
        print_pe32(&pe32);
    } while(Process32Next(hSnapshot, &pe32));
    CloseHandle(hSnapshot);
}

void type(const char *text) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    size_t len = strlen(text);
    for(size_t i = 0; i < len; ++i) {
        ip.ki.wVk = text[i];
        ip.ki.dwFlags = 0;
        SendInput(1, &ip, sizeof(INPUT));
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));
    }
}

void press_key(int keycode) {
    INPUT i = {0};
    i.type = INPUT_KEYBOARD;
    i.ki.wVk = keycode;
    SendInput(1, &i, sizeof(INPUT));
    i.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &i, sizeof(INPUT));
}

void press_ctrl_and(int keycode) {
    INPUT i[2];
    memset(i, 0, sizeof(INPUT) * 2);
    i[0].type = INPUT_KEYBOARD;
    i[1].type = INPUT_KEYBOARD;
    i[0].ki.wVk = VK_CONTROL;
    i[1].ki.wVk = keycode;
    SendInput(2, i, sizeof(INPUT));
    i[0].ki.dwFlags = KEYEVENTF_KEYUP;
    i[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, i, sizeof(INPUT));
}

void mouse_move(int dx, int dy) {
    INPUT i = {0};
    i.type = INPUT_MOUSE;
    i.mi.dwFlags = MOUSEEVENTF_MOVE;
    i.mi.dx = dx;
    i.mi.dy = dy;
    SendInput(1, &i, sizeof(INPUT));
}

void click_mouse_left() {
    INPUT i = {0};
    i.type = INPUT_MOUSE;
    i.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &i, sizeof(INPUT));
    i.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &i, sizeof(INPUT));
}

void click_mouse_right() {
    INPUT i = {0};
    i.type = INPUT_MOUSE;
    i.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    SendInput(1, &i, sizeof(INPUT));
    i.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    SendInput(1, &i, sizeof(INPUT));
}

HWND ResolveWindow(HWND hwnd) {
    HWND ret = NULL;
    if(GetWindowLong(hwnd, GWL_STYLE) & WS_CHILD) {
        ret = GetParent(hwnd);
    }
    if(ret == NULL) {
        ret = GetWindow(hwnd, GW_OWNER);
    }
    if(ret != NULL) {
        return ResolveWindow(ret);
    }
    return hwnd;
}