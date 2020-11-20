# TunkioCLI

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_executable("TunkioCLI" "Main.cpp" "TunkioCLI.cpp")
	target_precompile_headers("TunkioCLI" PRIVATE "TunkioCLI-PCH.hpp")
	target_link_libraries("TunkioCLI" LINK_PRIVATE TunkioAPI TunkioDEV)
else()
	add_executable("tunkio-cli" "Main.cpp" "TunkioCLI.cpp")
	target_precompile_headers("tunkio-cli" PRIVATE "TunkioCLI-PCH.hpp")
	target_link_libraries("tunkio-cli" LINK_PRIVATE tunkio-api tunkio-dev)
endif()
