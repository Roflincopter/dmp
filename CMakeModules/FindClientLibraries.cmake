# Qt 6 is the primary target; Qt 5.15 is still accepted as a fallback.
# Set DMP_QT_VERSION to 5 or 6 to pick one explicitly.
set(DMP_QT_VERSION "" CACHE STRING "Qt major version to build the client against (5 or 6, empty = auto)")

if(DMP_QT_VERSION)
	find_package(QT NAMES Qt${DMP_QT_VERSION} REQUIRED COMPONENTS Core)
else()
	find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
endif()
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

if(QT_VERSION VERSION_LESS 5.15)
	message(FATAL_ERROR "Qt 5.15 or Qt 6 is required, found Qt ${QT_VERSION}")
endif()

message(STATUS "Building the Qt client against Qt ${QT_VERSION}")
