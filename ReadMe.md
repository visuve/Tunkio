# Kuura

- Overwrites files, folders or drives
- Originally written during the small hours of a boring night
	- Since then it has bloated a bit...

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
	- ``git submodule update --init``

## Getting started

- ``apt install qt5-default``

## Tools

- Visual Studio >= 2019 or GCC >= 10.0 or Clang >= 10.0
- CMake >= 3.16

## Developer TODO

- Known bug: if file has no allocation size e.g. lies within MFT, the overwrite will fail
- Clarify the usage of words wipe, overwrite and pass within the code
- Complement fill option
	Complements the disk contents
- Allow continuing on errors
	- I.e. maybe have a boolean return value in OnError callback

## Further reading notes:

- https://linux.die.net/man/2/open
- https://www.ibm.com/support/knowledgecenter/STXKQY_5.0.5/com.ibm.spectrum.scale.v5r05.doc/bl1adm_considerations_direct_io.htm
- https://github.com/ronomon/direct-io
- https://lwn.net/Articles/457667/
- https://docs.microsoft.com/en-us/cpp/cpp/alignment-cpp-declarations?view=msvc-160
