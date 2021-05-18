# KuuraGUI

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	file(GLOB KuuraGUISources "*.cpp" "*.hpp" "*.ui" "*.qrc" "KuuraGUI.rc")
	qt_add_executable("KuuraGUI" ${KuuraGUISources} ${APP_ICON_RESOURCE_WINDOWS})
	target_precompile_headers("KuuraGUI" PRIVATE "KuuraGUI-PCH.hpp")
	target_link_libraries("KuuraGUI" LINK_PRIVATE Qt6::Widgets KuuraAPI KuuraDEV)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	file(GLOB KuuraGUISources "*.cpp" "*.hpp" "*.ui" "*.qrc")
	qt_add_executable("kuura-gui" ${KuuraGUISources})
	target_precompile_headers("kuura-gui" PRIVATE "KuuraGUI-PCH.hpp")
	target_link_libraries("kuura-gui" LINK_PRIVATE Qt6::Widgets kuura-api kuura-dev stdc++fs)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	file(GLOB KuuraGUISources "*.cpp" "*.hpp" "*.ui" "*.qrc")
	qt_add_executable("kuura-gui" ${KuuraGUISources})
	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_precompile_headers("kuura-gui" PRIVATE "KuuraGUI-PCH.hpp")
	endif()
	target_link_libraries("kuura-gui" LINK_PRIVATE Qt6::Widgets kuura-api kuura-dev c++)
endif()
