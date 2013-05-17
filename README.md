WyokiVNC project ... 

- Daemon -
Provides vncserver functionality, currently only tested it via adb shell, gui compiles fine, but start/stop activities not yet 100% ok :)
Available in jni/ folder

- Wrapper libs -
Compiled against the AOSP so everyone can build the daemon/GUI without having to fetch +2GB files.
Currently there are 2 wrappers, gralloc and flinger.

Available in nativeMethods/ folder, and precompiled libs in nativeMethods/lib/

- GUI - 
GUI handles user-friendly control.
Connects to the daemon using local IPC.

-------------- Compile C daemon ---------------------
On project folder:
  $ ndk-build
  $ ./updateExecsAndLibs.sh

-------------- Compile Wrapper libs -----------------
  $ cd <aosp_folder>
  $ . build/envsetup.sh
  $ lunch
  $ ln -s <wyoki-folder>/nativeMethods/ external/

To build:
  $ cd external/nativeMethods
  $ mm .
  $ cd <wyoki-folder>
  $ ./updateExecsAndLibs.sh

-------------- Compile GUI------- -------------------
Import using eclipse as a regular Android project
