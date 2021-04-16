cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)

project("Kuura")

if(EXISTS "${PROJECT_SOURCE_DIR}/googletest")
	add_subdirectory("googletest")
else()
	message(WARNING "Google Test not found, cannot compile unit tests. Please run git submodule update --init")
endif()

if(MSVC)
	add_compile_options(/W4 /WX /MP /wd26812)
else()
	add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

set(EXECUTABLE_OUTPUT_PATH "..")
set(LIBRARY_OUTPUT_PATH "..")

if(EXISTS "${PROJECT_SOURCE_DIR}/googletest")
	add_subdirectory("KuuraTests")
endif()

add_subdirectory("KuuraLIB")
add_subdirectory("KuuraAPI")
add_subdirectory("KuuraCLI")

# TEMPORARILY DISABLED FOR OTHER PLATFORMS. UBUNTU 20.04 (GITHUB CI VM) CANNOT INSTALL QT 5.15 WHICH IS A REQUIREMENT
if(CMAKE_SYSTEM_NAME MATCHES "Windows|FreeBSD")
	add_subdirectory("KuuraGUI")
endif()
