FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    python3-pip \
    libglib2.0-dev \
    libssl-dev \
    libpq-dev \
    uuid-dev \
    libwebsockets-dev \
    libc-ares-dev \
    coreutils \
    vim \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install aqtinstall

ENV QT_VERSION_FULL="6.7.0"
ENV QT_ARCH_AQT_INPUT="linux_gcc_64"
ENV QT_ARCH_DIR_NAME="gcc_64"

ENV QT_INSTALL_ROOT="/${QT_VERSION_FULL}/${QT_ARCH_DIR_NAME}"

ENV QT6_CMAKE_CONFIG_PATH="${QT_INSTALL_ROOT}/lib/cmake/Qt6"
ENV CMAKE_PREFIX_PATH="${QT6_CMAKE_CONFIG_PATH}${CMAKE_PREFIX_PATH:+:${CMAKE_PREFIX_PATH}}"

RUN echo "Installing Qt ${QT_VERSION_FULL} for ${QT_ARCH_AQT_INPUT} (directory ${QT_ARCH_DIR_NAME})..." && \
    echo "Expected Qt installation root: ${QT_INSTALL_ROOT}" && \
    aqt install-qt linux desktop ${QT_VERSION_FULL} ${QT_ARCH_AQT_INPUT} \
    --outputdir / \
    --modules qthttpserver qtwebsockets \
    && echo "Qt modules installation attempt finished."

RUN echo "--- Verifying Qt Installation Paths ---"
RUN ls -ld /${QT_VERSION_FULL} || echo "ls /${QT_VERSION_FULL} failed"
RUN ls -ld ${QT_INSTALL_ROOT} || echo "ls ${QT_INSTALL_ROOT} failed"
RUN echo "Searching for Qt6Config.cmake in ${QT_INSTALL_ROOT}..."
RUN find ${QT_INSTALL_ROOT} -name "Qt6Config.cmake" -print -quit || echo "Qt6Config.cmake not found via find in ${QT_INSTALL_ROOT}"
RUN echo "Explicitly checking for Qt6Config.cmake at ${QT6_CMAKE_CONFIG_PATH}/Qt6Config.cmake"
RUN if [ -f "${QT6_CMAKE_CONFIG_PATH}/Qt6Config.cmake" ]; then \
      echo "Qt6Config.cmake FOUND at ${QT6_CMAKE_CONFIG_PATH}/Qt6Config.cmake"; \
    else \
      echo "ERROR: Qt6Config.cmake NOT FOUND at ${QT6_CMAKE_CONFIG_PATH}/Qt6Config.cmake. Listing parent: ${QT_INSTALL_ROOT}/lib/cmake/"; \
      ls -lR "${QT_INSTALL_ROOT}/lib/cmake/" || echo "Could not list ${QT_INSTALL_ROOT}/lib/cmake/"; \
      exit 1; \
    fi
RUN echo "--- Qt Installation Path Verification Finished ---"

WORKDIR /tmp
ARG QTMQTT_CHECKOUT_TAG="v${QT_VERSION_FULL}"

RUN ls /tmp
RUN echo "Building qtmqtt from source, tag: ${QTMQTT_CHECKOUT_TAG}" && \
    echo "Qt installation path for qtmqtt (CMAKE_INSTALL_PREFIX): ${QT_INSTALL_ROOT}" && \
    echo "Path to Qt6 config for qtmqtt (Qt6_DIR): ${QT6_CMAKE_CONFIG_PATH}" && \
    \
    git clone https://github.com/qt/qtmqtt.git && \
    cd qtmqtt && \
    git checkout "${QTMQTT_CHECKOUT_TAG}" && \
    \
    mkdir build && \
    cd build && \
    \
    echo "Configuring qtmqtt with system cmake..." && \
    cmake .. \
        -GNinja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX="${QT_INSTALL_ROOT}" \
        -DQt6_DIR="${QT6_CMAKE_CONFIG_PATH}" && \
    \
    echo "Building qtmqtt..." && \
    cmake --build . --parallel $(nproc) && \
    \
    echo "Installing qtmqtt..." && \
    cmake --install . && \
    \
    echo "qtmqtt build and installation complete."

RUN ls /tmp
RUN ls /tmp/qtmqtt
#    && \
#    cd /tmp && \
#    rm -rf qtmqtt

RUN mkdir -p /build
WORKDIR /app
CMD ["/bin/sh", "./apibuilder.sh"]