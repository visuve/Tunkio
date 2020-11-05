# TunkioGUI

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

file(GLOB TunkioGUISources "*.cpp" "*.hpp" "*.ui" "*.qrc")

set(APP_ICON_RESOURCE_WINDOWS "../TunkioResources/Tunkio.rc")

if (MSVC)
	set(TunkioGUI "TunkioGUI")
else()
	set(TunkioGUI "tunkio_gui")
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
	qt_add_executable(${TunkioGUI} ${TunkioGUISources} ${APP_ICON_RESOURCE_WINDOWS})
else()
	add_executable(${TunkioGUI} ${TunkioGUISources} ${APP_ICON_RESOURCE_WINDOWS})
endif()

target_link_libraries(${TunkioGUI} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
