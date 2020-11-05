# TunkioDLL

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	file(GLOB TunkioDLLSource "TunkioAPICheck.c" "*.cpp" "Workloads/*.cpp" "FillProviders/*cpp" "Win32/*.cpp")
	set(TunkioDLL "TunkioAPI") # -> TunkioAPI.dll
else()
	file(GLOB TunkioDLLSource "TunkioAPICheck.c" "*.cpp" "Workloads/*.cpp" "FillProviders/*cpp" "Posix/*.cpp")
	set(TunkioDLL "tunkio-api") # -> tunkio-api.so
endif()

add_library(${TunkioDLL} SHARED ${TunkioDLLSource})
target_precompile_headers(${TunkioDLL} PRIVATE "PCH.hpp")
set_source_files_properties("TunkioAPICompliance.c" PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
set_target_properties(${TunkioDLL} PROPERTIES PUBLIC_HEADER "TunkioAPI.h")
target_include_directories(${TunkioDLL} PRIVATE "../TunkioLIB/")