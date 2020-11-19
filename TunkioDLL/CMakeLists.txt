# TunkioDLL

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	file(GLOB TunkioDLLSource "TunkioAPICheck.c" "*.cpp" "Workloads/*.cpp" "FillProviders/*cpp" "Win32/*.cpp")
	add_library("TunkioAPI" SHARED ${TunkioDLLSource})
	target_precompile_headers("TunkioAPI" PRIVATE "TunkioAPI-PCH.hpp")
	set_source_files_properties("TunkioAPICompliance.c" PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
	target_include_directories("TunkioAPI" PRIVATE "../TunkioLIB/")
	set_target_properties("TunkioAPI" PROPERTIES PUBLIC_HEADER "TunkioAPI.h")
else()
	file(GLOB TunkioDLLSource "TunkioAPICheck.c" "*.cpp" "Workloads/*.cpp" "FillProviders/*cpp" "Posix/*.cpp")
	set(TunkioDLL "tunkio-api") # -> tunkio-api.so
	add_library("tunkio-api" SHARED ${TunkioDLLSource})
	target_precompile_headers("tunkio-api" PRIVATE "TunkioAPI-PCH.hpp")
	set_source_files_properties("TunkioAPICompliance.c" PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
	target_include_directories("tunkio-api" PRIVATE "../TunkioLIB/")
	set_target_properties("tunkio-api" PROPERTIES PUBLIC_HEADER "TunkioAPI.h")
endif()
