# TunkioCLI

if(WIN32)
	file(GLOB TunkioCLISource "*.cpp" "Win32/*.cpp")
else()
	file(GLOB TunkioCLISource "*.cpp" "Posix/*.cpp")
endif()

add_executable(TunkioCLI ${TunkioCLISource})
target_precompile_headers(TunkioCLI PRIVATE "PCH.hpp")
target_include_directories(TunkioCLI PRIVATE "../TunkioDLL/")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	target_link_libraries(TunkioCLI LINK_PRIVATE TunkioDLL TunkioLIB stdc++fs)
else()
	target_link_libraries(TunkioCLI LINK_PRIVATE TunkioDLL TunkioLIB)
endif()