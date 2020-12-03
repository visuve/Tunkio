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

- Visual Studio >= 2019 or GCC >= 10.0 or Clang >= 10.0
- CMake >= 3.16

## Developer TODO

- The GUI is still somewhat flaky:
	- The GUI is not always updated when onProgress is emitted
	- The code needs cleaning up too
- Complement fill option
	Complements the disk contents
- Verifications
	- Char-by-char for charfiller
	- Sentence-by-sentece for file & sentence fillers
	- SHA-256 for random

## Further reading notes:

- https://linux.die.net/man/2/open
- https://www.ibm.com/support/knowledgecenter/STXKQY_5.0.5/com.ibm.spectrum.scale.v5r05.doc/bl1adm_considerations_direct_io.htm
- https://github.com/ronomon/direct-io
- https://lwn.net/Articles/457667/
- https://docs.microsoft.com/en-us/cpp/cpp/alignment-cpp-declarations?view=msvc-160
