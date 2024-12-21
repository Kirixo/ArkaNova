cmake_minimum_required(VERSION 3.16)

project(ArkaNova LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(COPY ${CMAKE_SOURCE_DIR}/config.ini DESTINATION ${CMAKE_BINARY_DIR})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Sql LinguistTools HttpServer Mqtt)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql LinguistTools HttpServer Mqtt)

set(TS_FILES ArkaNova_en_US.ts)

add_executable(ArkaNova
  main.cpp
  ${TS_FILES}
  jsonable.h
  measurement.h measurement.cpp
  logger.h logger.cpp
  dbcontroller.h dbcontroller.cpp
  responsefactory.h responsefactory.cpp
  routefactory.h routefactory.cpp
  servercontroller.h servercontroller.cpp
  mqttfactory.h mqttfactory.cpp
)

target_link_libraries(ArkaNova Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::HttpServer Qt${QT_VERSION_MAJOR}::Mqtt)

if(COMMAND qt_create_translation)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

include(GNUInstallDirs)
install(TARGETS ArkaNova
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
