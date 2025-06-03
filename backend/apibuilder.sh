#!/bin/bash

echo "--- apibuilder.sh ---"
echo "QT_INSTALL_ROOT is: ${QT_INSTALL_ROOT}"
echo "QT6_CMAKE_CONFIG_PATH is: ${QT6_CMAKE_CONFIG_PATH}"
echo "CMAKE_PREFIX_PATH from env is: ${CMAKE_PREFIX_PATH}"
echo "---"

find / -name "Qt6MqttConfig.cmake"
echo "------"
if [ ! -f /build/ArkaNova ]; then
  echo "Building the project..."
  cmake -B /build -S /app \
        -DQt6_DIR="${QT6_CMAKE_CONFIG_PATH}" \
        -DCMAKE_PREFIX_PATH="${QT_INSTALL_ROOT}" \
    && cmake --build /build

else
  echo "Project already built."
fi

if [ -f /build/ArkaNova ]; then
  echo "Running the application..."
  /build/ArkaNova
else
  echo "Build failed, application not found at /build/ArkaNova."
  exit 1
fi