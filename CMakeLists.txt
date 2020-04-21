#------------------------------------------------------------------------------
#
# \brief	CMakeLists.txt file for 'warehouse'
#
#------------------------------------------------------------------------------


cmake_minimum_required( VERSION 3.11 )
project ( warehouse )

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set( CMAKE_AUTOUIC_SEARCH_PATHS "ui" )

# Check either Qt6 or Qt5
find_package( QT NAMES Qt5 Qt6 COMPONENTS Core Widgets Network Sql REQUIRED )
find_package( Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Network Sql )

include_directories (
   src
   include
)

set (
   SRC
      src/warehouse.cpp
      src/tab.cpp
      src/main.cpp

      include/warehouse.hpp
      include/tab.hpp

      ui/warehouse.ui
      ui/tab.ui
      rc/warehouse.qrc
)

add_executable (
   ${PROJECT_NAME}
      ${SRC}
)

target_link_libraries (
   ${PROJECT_NAME}
      Qt${QT_VERSION_MAJOR}::Core
      Qt${QT_VERSION_MAJOR}::Widgets
      Qt${QT_VERSION_MAJOR}::Sql
)


#--- Fin ----------------------------------------------------------------------
