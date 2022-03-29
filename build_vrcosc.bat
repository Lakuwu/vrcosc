@echo off
if -%1-==-- echo Usage: %0 filename & goto end
set a=%1
set args=%1
shift
:argsloopstart
if -%1-==-- goto argsloopend
set args=%args% %1
shift
goto argsloopstart
:argsloopend

gcc -std=c18 -pedantic -Wall -Wextra -o %a:.c=.exe% %args% lib/vrc_osc.c lib/socket_util.c lib/windows_util.c lib/config.c -lws2_32

:end