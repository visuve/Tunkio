# TunkioGUI

set(CMAKE_CXX_STANDARD 17) # MSVC 16.8.2 + Qt 5.15.2 causes compilation errors on C++20
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

file(GLOB TunkioGUISources "*.cpp" "*.hpp" "*.ui" "*.qrc")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(APP_ICON_RESOURCE_WINDOWS "../TunkioResources/Tunkio.rc")

	if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
		qt_add_executable("TunkioGUI" ${TunkioGUISources} ${APP_ICON_RESOURCE_WINDOWS})
	else()
		add_executable("TunkioGUI" ${TunkioGUISources} ${APP_ICON_RESOURCE_WINDOWS})
	endif()

	target_precompile_headers("TunkioGUI" PRIVATE "TunkioGUI-PCH.hpp")
	target_link_libraries("TunkioGUI" LINK_PRIVATE Qt${QT_VERSION_MAJOR}::Widgets TunkioAPI TunkioDEV)
else()
	if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
		qt_add_executable("tunkio-gui" ${TunkioGUISources})
	else()
		add_executable("tunkio-gui" ${TunkioGUISources})
	endif()

	target_precompile_headers("tunkio-gui" PRIVATE "TunkioGUI-PCH.hpp")
	target_link_libraries("tunkio-gui" LINK_PRIVATE Qt${QT_VERSION_MAJOR}::Widgets tunkio-api tunkio-dev)
endif()
