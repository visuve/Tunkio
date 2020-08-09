cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)

project("Tunkio")

if(MSVC)
    add_compile_options(
        $<$<CONFIG:>:/MT>
        $<$<CONFIG:Debug>:/MTd>
        $<$<CONFIG:Release>:/MT>
    )
endif()

add_subdirectory("TunkioLIB")
add_subdirectory("TunkioDLL")
add_subdirectory("TunkioCLI")
add_subdirectory("TunkioGUI")
add_subdirectory("TunkioTests")
add_subdirectory("nana")
add_subdirectory("googletest")