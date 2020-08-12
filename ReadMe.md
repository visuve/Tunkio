# Tunkio

- Wipes files or unallocated volumes
- Originally written during the small hours of a boring night, since improved a lot
- Does not yet even try to be secure, done simply out of curiosity

## Folders

- ``TunkioLIB/``
	- Tunkio Static Library, contains various C++ wrappers for e.g. measuring time, getting process output, etc.
- ``TunkioDLL/``
	- Tunkio Shared Library, contains all the actual wiping logic and nothing else
	- C-compatible interface
	- Depengs on ``TunkioLIB``
- ``TunkioCLI/``
	- Tunkio Command Line Interface, shows nice progress output etc.
	- Depends on ``TunkioDLL`` & ``TunkioLIB``
- ``TunkioGUI/``
	- Tunkio Graphical User Interface, a simple dialog to show progress
	- Depends on ``TunkioDLL`` & ``TunkioLIB``

## Submodules

- Nana C++
  - https://github.com/cnjinhao/nana

## TODO

- Implementation
	- Add support to wipe directories

- Testing
	- Add also non other than "happy path" unit tests

## Tools

- Visual Studio >= 2019 or GCC >= 8.3.0
- CMake >= 3.17
