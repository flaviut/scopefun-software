# ScopeFun oscilloscope desktop

This is based upon the software found at http://www.scopefun.com, with some
minor tweaks.

In particular, I've:

- converted the build system from some weird hand-rolled thing to CMake,
- removed the vendor code & static linking in favor of using the system's
  libraries, and
- deleted the annoying "accept the license" nag screen on every startup

## Building

This software uses CMake as the build system. To compile the executable,
you'll need the stuff under "find_package" in CMakeLists.txt installed on your
system. IDK how this works on other OSes, but in Linux, this stuff can be found
in your package manager.

If I haven't fallen behind on updating this readme, that means:

- SDL2
- GLEW
- OpenGL
- WX
- libusb

Building is fairly typical for cmake:

```console
$ mkdir -p cmake-build-debug
$ cd cmake-build-debug
$ cmake ..
$ make -j$(nproc)
```

## Running

You can try running the program in any directory, but you'll get some cryptic
error and it'll immediately crash. This is because it wants the scope
calibration files. To fix this issue, make sure that your working directory is
`resources/` before you start the program.

```console
$ cd resources
$ ../cmake-build-debug/scopefun
```

### udev rules

You don't want to have to run the frontend as root, so add your user to the
uucp group (which will also give you access to serial devices), and copy
`resources/driver/51-scopefun.rules` into `/etc/udev/rules.d/`. [Reload udev
rules](https://unix.stackexchange.com/a/39371/70735) and re-plug-in the
oscilloscope, and you should be good to go.
