# TunkioLIB

if(WIN32)
	file(GLOB TunkioLIBSource "*.cpp" "Win32/*.cpp")
	add_library(TunkioLIB STATIC ${TunkioLIBSource})
	target_include_directories(TunkioLIB PUBLIC "." "Win32")
else()
	file(GLOB TunkioLIBSource "*.cpp" "Posix/*.cpp")
	add_library(TunkioLIB STATIC ${TunkioLIBSource})
	target_include_directories(TunkioLIB PUBLIC "." "Posix")
endif()