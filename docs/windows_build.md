# Windows Build Notes

# Dependencies
It is assumed that you have git, cmake, mingw and meson installed and that the midi-cube repository was cloned into a folder.
If any mingw packages are missing install them using the ming isntallation manager (e.g. mingw32-make)
Furthermore, kg-config needs to be isntalled: https://stackoverflow.com/questions/1710922/how-to-install-pkg-config-in-windows
The environment variable PKG_CONFIG_PATH must be set to "C:\Program Files (x86)"

# Install libraries
## RtAudio
Download: https://github.com/thestk/rtaudio/releases/tag/5.1.0
Tutorial: https://stackoverflow.com/questions/40193398/how-to-compile-rtaudio-on-windows-with-mingw
Notes:
* Including math.h is not necessary
* Choose "MinGW Makefiles" when configuring
* Select buildtype release
* Set RTAUDIO_ASIO to true and RTAUDIO_WASAPI to false
* Then execute mingw32-cmake install as admin

## RtAudio
Download: https://github.com/thestk/rtmidi/releases/tag/4.0.0
Same as above

## LibSndFile
Download (Win32 Build): https://github.com/libsndfile/libsndfile/releases/tag/v1.0.31
Extract into Program Files (x86) and name the folder libsndfile
Paste the following in C:\Program Files (x86)\libsndfile\lib\pkgconfig\sndfile.pc

```
prefix=C:/Program Files (x86)/libsndfile
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: libsndfile
Description: A library for reading and writing audio files
Version: 1.0.31
Requires:
Libs: -L${libdir} -lsndfile
Cflags: -I${includedir}
```

## SFML
Download: https://www.sfml-dev.org/download/sfml/2.5.1/
Extract into Program Files (x86) and name the folder SFML
Paste the following in C:\Program Files (x86)\SFML\lib\pkgconfig\sfml-system.pc
```
prefix=C:/Program Files (x86)/SFML
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: SFML System
Description: A C++ GUI library
Version: 2.5.1
Requires:
Libs: -L${libdir} -lsfml-system
Cflags: -I${includedir}
``` 
Paste the following in C:\Program Files (x86)\SFML\lib\pkgconfig\sfml-graphics.pc
```
prefix=C:/Program Files (x86)/SFML
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: SFML Graphics
Description: A C++ GUI library
Version: 2.5.1
Requires:
Libs: -L${libdir} -lsfml-graphics
Cflags: -I${includedir}
```
Paste the following in C:\Program Files (x86)\SFML\lib\pkgconfig\sfml-windows.pc
```
prefix=C:/Program Files (x86)/SFML
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: SFML Window
Description: A C++ GUI library
Version: 2.5.1
Requires:
Libs: -L${libdir} -lsfml-window
Cflags: -I${includedir}
```

## Boost
