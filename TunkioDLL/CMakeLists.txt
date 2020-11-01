# TunkioDLL

if(WIN32)
	file(GLOB TunkioDLLSource "TunkioAPICheck.c" "*.cpp" "Workloads/*.cpp" "FillProviders/*cpp" "Win32/*.cpp")
else()
	file(GLOB TunkioDLLSource "TunkioAPICheck.c" "*.cpp" "Workloads/*.cpp" "FillProviders/*cpp" "Posix/*.cpp")
endif()

add_library(TunkioDLL SHARED ${TunkioDLLSource})
target_precompile_headers(TunkioDLL PRIVATE "PCH.hpp")
set_source_files_properties("TunkioAPICompliance.c" PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
set_target_properties(TunkioDLL PROPERTIES PUBLIC_HEADER "TunkioAPI.h")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	target_link_libraries(TunkioDLL LINK_PRIVATE TunkioLIB stdc++fs)
else()
	target_link_libraries(TunkioDLL LINK_PRIVATE TunkioLIB)
endif()