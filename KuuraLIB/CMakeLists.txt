# KuuraLIB

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	file(GLOB KuuraLIBSource "*.cpp" "Win32/*.cpp")
	add_library("KuuraDEV" STATIC ${KuuraLIBSource}) # -> KuuraDEV.lib
	# target_precompile_headers("KuuraDEV" PRIVATE "KuuraLIB-PCH.hpp")
	target_link_libraries("KuuraDEV" wbemuuid)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	file(GLOB KuuraLIBSource "*.cpp" "Posix/*.cpp" "Linux/*.cpp")
	add_library("kuura-dev" STATIC  ${KuuraLIBSource}) # -> libkuura-dev.a
	target_precompile_headers("kuura-dev" PRIVATE "KuuraLIB-PCH.hpp")
	target_link_libraries("kuura-dev" udev stdc++fs)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	file(GLOB KuuraLIBSource "*.cpp" "Posix/*.cpp" "BSD/*.cpp")
	add_library("kuura-dev" STATIC  ${KuuraLIBSource}) # -> libkuura-dev.a
	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_precompile_headers("kuura-dev" PRIVATE "KuuraLIB-PCH.hpp")
	endif()
	target_link_libraries("kuura-dev" geom)
endif()
