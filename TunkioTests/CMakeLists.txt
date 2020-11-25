# TunkioTests

file(GLOB TunkioTestsSource
	"*.cpp"
	"../TunkioAPI/*.cpp"
	"../TunkioAPI/FillProviders/*.cpp"
	"../TunkioAPI/Workloads/TunkioWorkload.cpp"
	"../TunkioLIB/*.cpp"
	"../TunkioCLI/TunkioArgs.cpp")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_executable("TunkioTests" ${TunkioTestsSource})
	target_include_directories("TunkioTests" PRIVATE
		"../TunkioAPI/" "../TunkioLIB/" "../TunkioCLI/" "../googletest/googletest/include/")
	target_precompile_headers("TunkioTests" PRIVATE "TunkioTests-PCH.hpp")
	target_link_libraries("TunkioTests" gtest)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	add_executable("tunkio-tests" ${TunkioTestsSource})
	target_include_directories("tunkio-tests" PRIVATE
		"../TunkioAPI/" "../TunkioLIB/" "../TunkioCLI/" "../googletest/googletest/include/")
	target_precompile_headers("tunkio-tests" PRIVATE "TunkioTests-PCH.hpp")
	target_link_libraries("tunkio-tests" gtest stdc++fs)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	add_executable("tunkio-tests" ${TunkioTestsSource})
	target_include_directories("tunkio-tests" PRIVATE
		"../TunkioAPI/" "../TunkioLIB/" "../TunkioCLI/" "../googletest/googletest/include/")
	target_precompile_headers("tunkio-tests" PRIVATE "TunkioTests-PCH.hpp")
	target_link_libraries("tunkio-tests" gtest)
endif()
