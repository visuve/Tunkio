# Kuura

- Overwrites files, folders or drives
- Originally written during the small hours of a boring night
	- Since then has bloated a bit...

## Folders

- ``KuuraLIB/``
	- Kuura Static Library, contains shared stuff between GUI & CLI.
- ``KuuraAPI/``
	- Kuura Shared Library, contains all the actual wiping logic and nothing else
	- C-compatible interface
	- Depengs on ``KuuraLIB``
- ``KuuraCLI/``
	- Kuura Command Line Interface, shows nice progress output etc.
	- Depends on ``KuuraAPI`` & ``KuuraLIB``
- ``KuuraGUI/``
	- Kuura Graphical User Interface, a simple dialog to show progress
	- Depends on ``KuuraAPI`` & ``KuuraLIB``

## Submodules

- Google Test
	- https://github.com/google/googletest
		- Used for unit tests
## Getting started

1. git clone --recursive https://github.com/visuve/kuura.git
2. Install Qt
3. Run cmake
4. Build

## Tools

- Visual Studio >= 2019 or GCC >= 10.0 or Clang >= 10.0
- CMake >= 3.16
- Qt >= 5.15

## TODO:
- There is now regression in the GUI progress tab after refactoring
	- Functionality missing
- Known bug: if file has no allocation size e.g. lies within MFT, the overwrite will fail
	- This raises a question, why do I even care about the allocation size, caching or alignment with regular files as it's insecure to wipe them anyways?
		- Maybe just use std::fstream for files?
- Clarify the usage of words wipe, overwrite and pass within the code
- Complement fill option
	Complements the disk contents
- Allow continuing on errors
	- I.e. maybe have a boolean return value in OnError callback
