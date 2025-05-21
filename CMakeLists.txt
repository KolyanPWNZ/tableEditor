cmake_minimum_required(VERSION 3.14)

project(tableEditor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Xml)

set(SOURCES
    main.cpp
    mainwindow.cpp
    tablemodel.cpp
    delegates/numberdelegate.cpp
    delegates/stringdelegate.cpp
    delegates/datedelegate.cpp
    delegates/booldelegate.cpp
    parser/xmlparser.cpp
    serializer/xmlserializer.cpp
)

set(HEADERS
    mainwindow.h
    tablemodel.h
    delegates/numberdelegate.h
    delegates/stringdelegate.h
    delegates/datedelegate.h
    delegates/booldelegate.h
    parser/xmlparser.h
    serializer/xmlserializer.h
    columnTypeUtils.h
    NaturelSort.h
)

set(FORMS
    mainwindow.ui
)

add_executable(tableEditor
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
)

if(WIN32)
    set_target_properties(tableEditor PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

set_target_properties(tableEditor PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.tableEditor
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

target_link_libraries(tableEditor PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Xml
)

include(GNUInstallDirs)

install(TARGETS tableEditor
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
