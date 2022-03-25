# vrcosc
A simple C library for interacting with VRChat through OSC and some programs utilizing it.

# Building
1. Have `gcc` in PATH
2. Run `build_vrcosc.bat filename`, for example: `build_vrcosc.bat paste_vrc.c`
3. ???
4. Profit

# Configuration
The programs have a default configuration that you can override with a configuration file. For example, if you want to override the configuration of `paste_vrc.exe` you should create an empty file called `paste_vrc.ini` in the folder where the program is. Running the program will then populate the configuration file with the default values. Any changes you have made will be applied the next time you run the program.

# paste_vrc
Allows you to paste text into VRChat.

Requires 1 boolean parameter and an expression menu button.

Default configuration:
- listening port: 9001
- paste parameter: `/avatar/parameters/paste`
