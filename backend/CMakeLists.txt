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
  controllers/dbcontroller.cpp controllers/dbcontroller.h
  controllers/servercontroller.cpp controllers/servercontroller.h
  models/measurement.cpp models/measurement.h routes/mqttfactory.cpp
  routes/mqttfactory.h routes/routefactory.cpp routes/routefactory.h
  utils/jsonable.h
  utils/logger.cpp utils/logger.h
  utils/responsefactory.cpp utils/responsefactory.h
  models/user.h models/user.cpp
  repositories/userrepository.h repositories/userrepository.cpp
  routes/userhandler.h routes/userhandler.cpp
  routes/sensorhandler.h routes/sensorhandler.cpp
  repositories/sensorrepository.h repositories/sensorrepository.cpp
  models/sensor.h models/sensor.cpp
  models/sensortype.h models/sensortype.cpp
  repositories/sensortyperepository.h repositories/sensortyperepository.cpp
  models/solarpanel.h models/solarpanel.cpp
  repositories/solarpanelrepository.h repositories/solarpanelrepository.cpp
  repositories/measurementrepository.h repositories/measurementrepository.cpp
  routes/solarpanelhandler.h routes/solarpanelhandler.cpp
  routes/measurementhandler.h routes/measurementhandler.cpp
  routes/mqttmeasurementhandler.h routes/mqttmeasurementhandler.cpp
  routes/backuphandler.h routes/backuphandler.cpp
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
