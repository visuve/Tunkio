# TunkioDLL

if(WIN32)
	file(GLOB TunkioDLLSource "TunkioAPICheck.c" "*.cpp" "Win32/*.cpp")
else()
	file(GLOB TunkioDLLSource "TunkioAPICheck.c" "*.cpp" "Posix/*.cpp")
endif()

add_library(TunkioDLL SHARED ${TunkioDLLSource})

target_precompile_headers(TunkioDLL PRIVATE "PCH.hpp")
set_source_files_properties("TunkioAPICheck.c" PROPERTIES SKIP_PRECOMPILE_HEADERS ON)

set_target_properties(TunkioDLL PROPERTIES PUBLIC_HEADER "TunkioAPI.h")
target_link_libraries(TunkioDLL LINK_PRIVATE TunkioLIB)