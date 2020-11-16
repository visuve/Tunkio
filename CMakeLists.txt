cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)

project("Tunkio")

add_subdirectory("googletest")

if(MSVC)
	add_compile_options(/W4 /WX)
else()
	add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_subdirectory("TunkioLIB")
add_subdirectory("TunkioDLL")
add_subdirectory("TunkioCLI")
# add_subdirectory("TunkioGUI")
add_subdirectory("TunkioTests")