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

gcc -std=c18 -pedantic -Wall -Wextra -o %a:.c=.exe% %args% vrc_osc.c socket_util.c windows_util.c config.c -lws2_32

:end