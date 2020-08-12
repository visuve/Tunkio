# TunkioGUI

file(GLOB TunkioGUISource "*.cpp")

if(WIN32)
	add_executable(TunkioGUI WIN32 ${TunkioGUISource})
else()
	add_executable(TunkioGUI ${TunkioGUISource})
endif()

target_include_directories(TunkioGUI PRIVATE "../TunkioDLL/") # TODO: This is shit

target_link_libraries(TunkioGUI LINK_PRIVATE TunkioDLL TunkioLIB nana)