# KuuraCLI

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	file(GLOB KuuraCLISource "*.cpp" "KuuraCLI.rc")
	add_executable("KuuraCLI" ${KuuraCLISource})
	target_precompile_headers("KuuraCLI" PRIVATE "KuuraCLI-PCH.hpp")
	target_link_libraries("KuuraCLI" LINK_PRIVATE KuuraAPI KuuraDEV)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	file(GLOB KuuraCLISource "*.cpp")
	add_executable("kuura-cli" ${KuuraCLISource})
	target_precompile_headers("kuura-cli" PRIVATE "KuuraCLI-PCH.hpp")
	target_link_libraries("kuura-cli" LINK_PRIVATE kuura-api kuura-dev)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	file(GLOB KuuraCLISource "*.cpp")
	add_executable("kuura-cli" ${KuuraCLISource})
	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_precompile_headers("kuura-cli" PRIVATE "KuuraCLI-PCH.hpp")
	endif()
	target_link_libraries("kuura-cli" LINK_PRIVATE kuura-api kuura-dev)
endif()
