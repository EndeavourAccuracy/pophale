===[CONTENTS]==================================================================

1 - ABOUT
2 - LICENSE/DISCLAIMER
3 - USAGE
4 - FEEDBACK
5 - DID YOU CREATE NEW LEVELS?
6 - (RE)COMPILING

===[1 - ABOUT]=================================================================

pophale v0.9b (June 2018)
Copyright (C) 2018 Norbert de Jonge <mail@norbertdejonge.nl>

A level editor of Prince of Persia: Harem Adventures.
The pophale website can be found at [ https://www.norbertdejonge.nl/pophale/ ].

The game is a MIDlet for the Java ME (J2ME/phoneME) environment.
Supported is the 176x208 version without Nokia UI API.

===[2 - LICENSE/DISCLAIMER]====================================================

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see [ www.gnu.org/licenses/ ].

-----

The above license is only for pophale itself; its source code and its images.

Prince of Persia is © Jordan Mechner/Ubisoft.
Harem Adventures was created by Gameloft/Vivendi.
The Bitstream Vera font by Jim Lyles is © Bitstream, Inc.
The Xbox icons are © Jeff Jenkins, CC BY 3.0.
MicroEmulator is © Bartek Teodorczyk et al., LGPL2.1+/AL2.

===[3 - USAGE]=================================================================

scroll wheel (1-7) = change zoom level
click, and hold down, left = drag level
click left (Enter) = change back tile
click middle (DEL) = remove front tile
click right (INS) = add front tile

NOTE: Removal happens in two steps. First, the nearest tile is marked. Then, if the cursor is not moved, that tile is removed.

===[4 - FEEDBACK]==============================================================

If pophale crashes, gets compilation errors or crashes while building, send an e-mail to [ mail@norbertdejonge.nl ]. Make sure to describe exactly what actions triggered the bug and the precise symptoms of the bug. If possible, also include: 'uname -a', 'gcc -v', 'sdl2-config --version', and 'pophale --version'.

===[5 - DID YOU CREATE NEW LEVELS?]============================================

Feel free to share your work:
http://forum.princed.org/

===[6 - (RE)COMPILING]=========================================================

GNU/Linux
=========

Prerequisites:
- libsdl2-dev, libsdl2-image-dev and libsdl2-ttf-dev
- libzip-dev

$ make

Windows
=======

The following instructions are to compile 32-bit.

1) Set up Dev-C++:

1.1 Download

http://sourceforge.net/projects/orwelldevcpp/files/Setup%20Releases/
or
http://downloads.sourceforge.net/project/orwelldevcpp/Setup%20Releases/
  Dev-Cpp%205.11%20TDM-GCC%204.9.2%20Setup.exe

1.2 Install

Simply run the executable.

2) Install SDL2's MinGW libraries:

2.1 Download

http://libsdl.org/release/
  SDL2-devel-2.0.7-mingw.tar.gz
http://libsdl.org/projects/SDL_ttf/release/
  SDL2_ttf-devel-2.0.14-mingw.tar.gz
http://libsdl.org/projects/SDL_image/release/
  SDL2_image-devel-2.0.2-mingw.tar.gz

2.2 Install

Unpack the packages.

For all three packages, copy the i686-w64-mingw32/ directories into the Dev-Cpp/MinGW64/ directory. Make sure to use the zlib1.dll from SDL2_image (and NOT from SDL2_ttf).

GNU/Linux users who use Wine can find this directory at:
~/.wine/drive_c/Program Files (x86)/Dev-Cpp/MinGW64/

Copy the Dev-Cpp/MinGW64/i686-w64-mingw32/bin/*.dll files to the pophale directory.
(You do not need libjpeg-9.dll, libtiff-5.dll and libwebp-7.dll.)

3) Install libzip's MinGW library:

3.1 Download

https://kojipkgs.fedoraproject.org//packages/mingw-libzip/1.1.3/1.fc25/noarch/mingw32-libzip-1.1.3-1.fc25.noarch.rpm

(If you use a newer version, libzip-5.dll will complain about a missing libbz2-1.dll file.)

3.2 Install

Use 7-Zip to unpack the RPM file.
On GNU/Linux, that is:
$ 7z x mingw32-libzip-1.1.3-1.fc25.noarch.rpm
$ cpio -idv < mingw32-libzip-1.1.3-1.fc25.noarch.cpio

Copy its files from
usr/i686-w64-mingw32/sys-root/mingw/
to
Dev-Cpp/MinGW64/i686-w64-mingw32/

You may need to copy
Dev-Cpp/MinGW64/i686-w64-mingw32/lib/libzip/include/zipconf.h
over
Dev-Cpp/MinGW64/i686-w64-mingw32/include/zipconf.h

From
Dev-Cpp/MinGW64/i686-w64-mingw32/bin/
copy its
- libzip-4.dll
file to the pophale directory.

4) Add additional DLL files

Download (and install)
https://download.gimp.org/mirror/pub/gimp/v2.8/windows/gimp-2.8.22-setup.exe
and copy its
- 32/bin/libgcc_s_sjlj-1.dll
- 32/bin/libwinpthread-1.dll
files to the pophale directory.

5) Compile

Start Dev-C++.

Go to: File->New->Project...
Basic->Console Application
C Project
Name: pophale

Go to: Project->Remove From Project...
Select main.c and press Delete.

Project->Add To Project...
pophale.c

Go to: Project->Project Options...->Compiler
Set "Base compiler set:" to "TDM-GCC 4.9.2 32-bit Release".
(Discard customizations if necessary.)

Go to: Project->Project Options...->Parameters
In the C compiler field, add:
-m32 -O2 -Wno-unused-result -std=c99 -pedantic -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -D_REENTRANT -lm -lzip
In the Linker field, add:
-l"mingw32"
-l"SDL2main"
-l"SDL2.dll"
-l"SDL2_image.dll"
-l"SDL2_ttf.dll"
-l"zip.dll"

Go to: Project->Project Options...->Directories
Select the tab: Include Directories
Add: C:\Program Files (x86)\Dev-Cpp\MinGW64\i686-w64-mingw32\include\SDL2
Add: C:\Program Files (x86)\Dev-Cpp\MinGW64\i686-w64-mingw32\include
Select the tab: Library Directories
Add: C:\Program Files (x86)\Dev-Cpp\MinGW64\i686-w64-mingw32\lib

Go to: Tools->Compiler Options...->Directories
Select the tab: Binaries
Add: C:\Program Files (x86)\Dev-Cpp\MinGW64\i686-w64-mingw32\bin
Select the tab: Libraries
Add: C:\Program Files (x86)\Dev-Cpp\MinGW64\i686-w64-mingw32\lib

Go to: Project->Project Options...->General
Browse and add: png/various/pophale_icon.ico

Select: Execute->Compile (or press F9).
