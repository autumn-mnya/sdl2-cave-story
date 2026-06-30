# Cave Story but with SDL Graphics (autumn's fork)

## Introduction (from autumn)

Hi! This is a fork of periwinkles SDL2 dll for Cave Story, adding 60fps, a function to draw bitmaps with alpha, and more surfaces for the user to use alongside AutPI.

This version also __requires__ AutPI to be installed.

## Introduction (from Periwinkle)
This is a DLL mod that completely rips out all of Cave Story's DirectDraw rendering system,
and replaces it with SDL calls (based on CSE2-portable backend code).
It is intended to fix issues such as sprites not drawing with transparency, or fullscreen
not working. It is *not* intended to be modding-friendly, although it may still be
compatible with mods that don't touch any of the rendering code.

**THIS IS NOT A GENERAL-PURPOSE DDRAW.DLL WRAPPER.** Don't try to use this for other games!

## Usage
Download a release and unzip next to Doukutsu.exe.

Note the following changes to the behavior of the DoConfig options:
* The "Full 24-bit" window mode will be treated as "960x720 Windowed"
* The "Full 32-bit" window mode will be treated as "1280x960 Windowed"
* Courier New is the only available font; changing the font name in Config.dat will have no effect.

## Building

The recommended method of compiling this mod is to use [CMake](https://cmake.org/).

For windows users, I would recommend installing CMake, alongside __Visual Studio 2019__ or above. Install both, open "cmake-gui" if inexperienced, open the source code folder, and set a build directory. Configure it, and set the platform to "Win32". Generate after, and open the new `.sln` file in the build folder, and build!

## Compatibility

This dll should work for Windows 7+, but not XP or below.