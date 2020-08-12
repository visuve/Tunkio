# TunkioLIB

if(WIN32)
	file(GLOB TunkioLIBSource "*.cpp" "Win32/*.cpp")
	add_library(TunkioLIB STATIC ${TunkioLIBSource})
	target_precompile_headers(TunkioLIB PRIVATE "PCH.hpp")
	target_include_directories(TunkioLIB PUBLIC "." "Win32")
else()
	file(GLOB TunkioLIBSource "*.cpp" "Posix/*.cpp")
	add_library(TunkioLIB STATIC ${TunkioLIBSource})
	target_precompile_headers(TunkioLIB PRIVATE "PCH.hpp")
	target_include_directories(TunkioLIB PUBLIC "." "Posix")
endif()