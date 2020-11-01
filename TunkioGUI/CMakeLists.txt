# cmake_minimum_required(VERSION 3.8)

# project(TunkioGUI LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

file(GLOB TUNKIO_SOURCES "*.cpp" "*.hpp" ".ui" "*.qrc")

set(APP_ICON_RESOURCE_WINDOWS "../TunkioResources/Tunkio.rc")

if (MSVC)
	set(TUNKIO_EXECUTABLE "TunkioGUI")
else()
	set(TUNKIO_EXECUTABLE "tunkio_gui")
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
	qt_add_executable(${TUNKIO_EXECUTABLE} ${TUNKIO_SOURCES} ${APP_ICON_RESOURCE_WINDOWS})
else()
	add_executable(${TUNKIO_EXECUTABLE} ${TUNKIO_SOURCES} ${APP_ICON_RESOURCE_WINDOWS})
endif()

target_link_libraries(${TUNKIO_EXECUTABLE} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
