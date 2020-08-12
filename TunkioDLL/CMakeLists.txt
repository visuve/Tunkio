# TunkioDLL

if(WIN32)
	file(GLOB TunkioDLLSource "*.c" "*.cpp" "Win32/*.cpp")
else()
	file(GLOB TunkioDLLSource "*.c" "*.cpp" "Posix/*.cpp")
endif()

add_library(TunkioDLL SHARED ${TunkioDLLSource})
set_target_properties(TunkioDLL PROPERTIES PUBLIC_HEADER "TunkioAPI.h")
target_link_libraries(TunkioDLL LINK_PRIVATE TunkioLIB)