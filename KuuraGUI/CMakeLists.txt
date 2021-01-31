# KuuraGUI

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

file(GLOB KuuraGUISources "*.cpp" "*.hpp" "*.ui" "*.qrc")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(APP_ICON_RESOURCE_WINDOWS "../KuuraResources/Kuura.rc")

	if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
		qt_add_executable("KuuraGUI" ${KuuraGUISources} ${APP_ICON_RESOURCE_WINDOWS})
	else()
		add_executable("KuuraGUI" ${KuuraGUISources} ${APP_ICON_RESOURCE_WINDOWS})
	endif()

	target_precompile_headers("KuuraGUI" PRIVATE "KuuraGUI-PCH.hpp")
	target_link_libraries("KuuraGUI" LINK_PRIVATE Qt${QT_VERSION_MAJOR}::Widgets KuuraAPI KuuraDEV)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
		qt_add_executable("kuura-gui" ${KuuraGUISources})
	else()
		add_executable("kuura-gui" ${KuuraGUISources})
	endif()
	target_precompile_headers("kuura-gui" PRIVATE "KuuraGUI-PCH.hpp")
	target_link_libraries("kuura-gui" LINK_PRIVATE Qt${QT_VERSION_MAJOR}::Widgets kuura-api kuura-dev)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
		qt_add_executable("kuura-gui" ${KuuraGUISources})
	else()
		add_executable("kuura-gui" ${KuuraGUISources})
	endif()
	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_precompile_headers("kuura-gui" PRIVATE "KuuraGUI-PCH.hpp")
	endif()
	target_link_libraries("kuura-gui" LINK_PRIVATE Qt${QT_VERSION_MAJOR}::Widgets kuura-api kuura-dev c++)
endif()