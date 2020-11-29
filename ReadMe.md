# Tunkio

- Wipes files, folders or drives
- Originally written during the small hours of a boring night
	- Since then it has bloated a bit

## Folders

- ``TunkioLIB/``
	- Tunkio Static Library, contains shared stuff between GUI & CLI.
- ``TunkioAPI/``
	- Tunkio Shared Library, contains all the actual wiping logic and nothing else
	- C-compatible interface
	- Depengs on ``TunkioLIB``
- ``TunkioCLI/``
	- Tunkio Command Line Interface, shows nice progress output etc.
	- Depends on ``TunkioAPI`` & ``TunkioLIB``
- ``TunkioGUI/``
	- Tunkio Graphical User Interface, a simple dialog to show progress
	- Depends on ``TunkioAPI`` & ``TunkioLIB``

## Submodules

- Google Test
	- https://github.com/google/googletest
		- Used for unit tests
	- ``git submodule update --init``

## Getting started

- ``apt install qt5-default``

## Tools

- Visual Studio >= 2019 or GCC >= 9.3 or Clang >= 10.0
- CMake >= 3.16

## Developer TODO

- The GUI is still somewhat flaky
	- The code needs cleaning up too
- Rewind pointers to zero after a pass
- Verifications
	- Char-by-char for charfiller
	- Sentence-by-sentece for file & sentence fillers
	- SHA-256 for random
- Preset "recipes", i.e:
	- https://en.wikipedia.org/wiki/Data_erasure#Standards