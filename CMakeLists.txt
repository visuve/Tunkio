cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)

project("Tunkio")

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

if(EXISTS "${PROJECT_SOURCE_DIR}/googletest")
	add_subdirectory("TunkioTests")
endif()

add_subdirectory("TunkioLIB")
add_subdirectory("TunkioAPI")
add_subdirectory("TunkioCLI")
add_subdirectory("TunkioGUI")
