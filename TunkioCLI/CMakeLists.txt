# TunkioCLI

file(GLOB TunkioCLISource "*.cpp")
add_executable(TunkioCLI ${TunkioCLISource})

target_include_directories(TunkioCLI PRIVATE "../TunkioDLL/") # TODO: This is shit

target_link_libraries(TunkioCLI LINK_PRIVATE TunkioDLL TunkioLIB)