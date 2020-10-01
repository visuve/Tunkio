# TunkioLIB

if(WIN32)
	file(GLOB TunkioLIBSource "*.cpp" "Win32/*.cpp")
else()
	file(GLOB TunkioLIBSource "*.cpp" "Posix/*.cpp")
endif()

add_library(TunkioLIB STATIC ${TunkioLIBSource})
target_precompile_headers(TunkioLIB PRIVATE "PCH.hpp")

if(WIN32)
	target_include_directories(TunkioLIB PUBLIC "." "Win32")
else()
	target_include_directories(TunkioLIB PUBLIC "." "Posix")
endif()