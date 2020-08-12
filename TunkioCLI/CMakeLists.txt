# TunkioCLI

file(GLOB TunkioCLISource "*.cpp")
add_executable(TunkioCLI ${TunkioCLISource})

target_precompile_headers(TunkioCLI PRIVATE "PCH.hpp")

target_include_directories(TunkioCLI PRIVATE "../TunkioDLL/") # TODO: This is shit

if(WIN32)
	target_link_libraries(TunkioCLI LINK_PRIVATE TunkioDLL TunkioLIB)
else()
	target_link_libraries(TunkioCLI LINK_PRIVATE TunkioDLL TunkioLIB stdc++fs)
endif()