ByteImage Library by Brian Jackson
==================================

Contents
--------

1. License terms
2. Installation
3. Usage
4. Contact Information

1. License Terms
----------------

Please see LICENSE.md for licensing terms of this package.

This library optionally makes use of SDL, OpenCV, and FreeType packages. Please consult their distribution packages for any additional restriction on this library's use in software.

2. Installation
---------------

To compile this project, a cc-compatible compiler with C++11 support, such as g++, is required.

cd into the libbyteimage directory and execute ` make &&& sudo make install`

Then type `byteimage-config` to verify that the package is correctly installed.

If SDL2 is installed on your system, a suite of basic UI tools will be installed. If OpenCV is installed, video read/write tools and conversion routines to OpenCV2 structures will be installed, and if FreeType is supported, text rendering and on-screen display tools will be installed.

Execute `make tests` after installing libbyteimage to build a series of tests showing off installed functionality.

3. Usage
--------

When compiling against libbyteimage, use the byteimage-config utility to set compiler flags.

`byteimage-config --cflags` prints a list of all compilation flags

`byteimage-config --libs` prints all necessary linker flags

`byteimage-config --inc` prints a list of installed header files

Header files are the best source of information on using the libbyteimage classes (for now).

4. Contact Information
----------------------

ByteImage is maintained by Brian Jackson. You can find the newest source at [GitHub](https://github.com/axnjaxn/libbyteimage), and contact Dr. Jackson at axnjaxn AT axnjaxn DOT com.