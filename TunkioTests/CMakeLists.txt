# TunkioTests

file(GLOB TunkioTestsSource "*.cpp", "../TunkioDLL/*.cpp")
add_executable(TunkioTests ${TunkioTestsSource})

target_include_directories(TunkioTests PRIVATE "../TunkioDLL/") # TODO: This is shit
target_include_directories(TunkioTests PRIVATE "../googletest/googletest/include/") # TODO: This is shit

target_link_libraries(TunkioTests LINK_PRIVATE TunkioLIB gtest)