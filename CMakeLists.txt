cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

find_package(Qt6)

if(Qt6_FOUND)
	add_subdirectory("KuuraGUI")
else()
	message(WARNING "QT 6 not found. KuuraGUI not included.")
endif()
