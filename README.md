# vrcosc
A simple C library for interacting with VRChat through OSC and some programs utilizing it.

# paste_vrc
![paste_vrc GIF](https://media.giphy.com/media/v4LZiPuIntIPDDhfxG/giphy.gif)

Allows you to paste text into VRChat.

### Setup
You need to set up your [Expression Menu](https://docs.vrchat.com/docs/expression-menu-and-controls) for your avatar:
1. Add a new Expression Parameter with the name `paste` and the type `Bool`.
 
![paste expression parameter](/res/paste_parameter.png)

2. Add a new Expression Menu control with the type `Button` and select the Parameter you just created.

![paste expression menu](/res/paste_menu.png)

Make sure you have the correct avatar selected at the top the Expression Menu inspector, and that you have assigned both the menu and the parameters in the avatar descriptor!

Then, when you're in VRChat in your updated avatar you need to open your [Action Menu](https://docs.vrchat.com/docs/action-menu) and navigate to Options -> OSC and click Reset Config. This will update the OSC [config](https://docs.vrchat.com/docs/osc-avatar-parameters) for that avatar to include the parameter you added. While you're in the OSC menu make sure it's enabled!

That's it! Now all you have to do is to have `paste_vrc.exe` running in the background while you're in VRChat and you can enjoy a slightly more convenient way of copy-pasting video URLs or anything else into VRChat.

# Configuration
The programs have a default configuration that you can override with a configuration file. For example, if you want to override the configuration of `paste_vrc.exe` you should create an empty file called `paste_vrc.ini` in the folder where the program is. Running the program will then populate the configuration file with the default values. Any changes you have made will be applied the next time you run the program.

# Building
1. Have `gcc` in PATH
2. Run `build_vrcosc.bat filename`, for example: `build_vrcosc.bat paste_vrc.c`
3. ???
4. Profit
