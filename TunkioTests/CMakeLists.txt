# TunkioTests

file(GLOB TunkioTestsSource "*.cpp" "../TunkioDLL/TunkioAPI.cpp" "../TunkioDLL/TunkioFillGenerator.cpp")
add_executable(TunkioTests ${TunkioTestsSource})

target_precompile_headers(TunkioTests PRIVATE "PCH.hpp")

target_include_directories(TunkioTests PRIVATE "../TunkioDLL/")
target_include_directories(TunkioTests PRIVATE "../googletest/googletest/include/")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	target_link_libraries(TunkioTests LINK_PRIVATE TunkioLIB gtest stdc++fs)
else()
	target_link_libraries(TunkioTests LINK_PRIVATE TunkioLIB gtest)
endif()
