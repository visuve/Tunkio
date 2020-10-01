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
- Google Test
  - https://github.com/google/googletest

## Tools

- Visual Studio >= 2019 or GCC >= 8.3.0 or Clang >= 7.0.1
- CMake >= 3.17

## TODO:

- Testing
	- Add other than "happy path" unit tests
