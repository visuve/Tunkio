# TunkioLIB

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	file(GLOB TunkioLIBSource "*.cpp" "Win32/*.cpp")
	add_library("TunkioDEV" STATIC ${TunkioLIBSource}) # -> TunkioDEV.lib
	# target_precompile_headers("TunkioDEV" PRIVATE "TunkioLIB-PCH.hpp")
	target_link_libraries("TunkioDEV" wbemuuid)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	file(GLOB TunkioLIBSource "*.cpp" "Posix/*.cpp" "Linux/*.cpp")
	add_library("tunkio-dev" STATIC  ${TunkioLIBSource}) # -> libtunkio-dev.a
	target_precompile_headers("tunkio-dev" PRIVATE "TunkioLIB-PCH.hpp")
	target_link_libraries("tunkio-dev" udev stdc++fs)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	file(GLOB TunkioLIBSource "*.cpp" "Posix/*.cpp" "BSD/*.cpp")
	add_library("tunkio-dev" STATIC  ${TunkioLIBSource}) # -> libtunkio-dev.a
	target_precompile_headers("tunkio-dev" PRIVATE "TunkioLIB-PCH.hpp")
	target_link_libraries("tunkio-dev" geom)
endif()
