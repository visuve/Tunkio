# TunkioTests

file(GLOB TunkioTestsSource
	"*.cpp"
	"../TunkioDLL/*.cpp"
	"../TunkioDLL/FillProviders/*.cpp"
	"../TunkioLIB/*.cpp")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_executable("TunkioTests" ${TunkioTestsSource})
	target_include_directories("TunkioTests" PRIVATE "../TunkioDLL/" "../TunkioLIB/" "../googletest/googletest/include/")
	target_precompile_headers("TunkioTests" PRIVATE "TunkioTests-PCH.hpp")
	target_link_libraries("TunkioTests" gtest)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	add_executable("tunkio-tests" ${TunkioTestsSource})
	target_include_directories("tunkio-tests" PRIVATE "../TunkioDLL/" "../TunkioLIB/" "../googletest/googletest/include/")
	target_precompile_headers("tunkio-tests" PRIVATE "TunkioTests-PCH.hpp")
	target_link_libraries("tunkio-tests" gtest stdc++fs)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	add_executable("tunkio-tests" ${TunkioTestsSource})
	target_include_directories("tunkio-tests" PRIVATE "../TunkioDLL/" "../TunkioLIB/" "../googletest/googletest/include/")
	target_precompile_headers("tunkio-tests" PRIVATE "TunkioTests-PCH.hpp")
	target_link_libraries("tunkio-tests" gtest)
endif()
