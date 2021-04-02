# KuuraTests

file(GLOB KuuraTestsSource
	"*.cpp"
	"TestListeners/*.cpp"
	"../KuuraAPI/KuuraAPI.cpp"
	"../KuuraAPI/KuuraComposer.cpp"
	"../KuuraAPI/FillProviders/*.cpp"
	"../KuuraAPI/Workloads/*.cpp"
	"../KuuraLIB/*.cpp"
	"../KuuraCLI/KuuraArgument.cpp")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		list(APPEND KuuraTestsSource "TestListeners/Win32/KuuraWin32MemoryLeakListener.cpp")
	endif()

	add_executable("KuuraTests" ${KuuraTestsSource})
	target_include_directories("KuuraTests" PRIVATE
		"../KuuraAPI/" "../KuuraLIB/" "../KuuraCLI/" "../googletest/googletest/include/")
	target_precompile_headers("KuuraTests" PRIVATE "KuuraTests-PCH.hpp")
	target_link_libraries("KuuraTests" gtest)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	add_executable("kuura-tests" ${KuuraTestsSource})
	target_include_directories("kuura-tests" PRIVATE
		"../KuuraAPI/" "../KuuraLIB/" "../KuuraCLI/" "../googletest/googletest/include/")
	target_precompile_headers("kuura-tests" PRIVATE "KuuraTests-PCH.hpp")
	target_link_libraries("kuura-tests" gtest stdc++fs)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	add_executable("kuura-tests" ${KuuraTestsSource})
	target_include_directories("kuura-tests" PRIVATE
		"../KuuraAPI/" "../KuuraLIB/" "../KuuraCLI/" "../googletest/googletest/include/")
	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_precompile_headers("kuura-tests" PRIVATE "KuuraTests-PCH.hpp")
	endif()
	target_link_libraries("kuura-tests" gtest)
endif()
