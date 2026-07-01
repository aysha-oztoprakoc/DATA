cmake_minimum_required(VERSION 3.15)
project(KAD_TELL CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror=return-type -pthread")

# Include Conan dependencies (assuming conan install .. --build=missing)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake OPTIONAL)
if(COMMAND conan_basic_setup)
    conan_basic_setup()
endif()

# Find external packages (if not using Conan or falling back to system packages)
find_package(OpenSSL REQUIRED)
# Paho MQTT C and C++ libraries are required
# find_package(paho-mqtt-c REQUIRED)
# find_package(paho-mqtt-cpp REQUIRED)
# nlohmann_json is header-only but finding it is good practice
# find_package(nlohmann_json REQUIRED)

# Include directories
include_directories(src)
include_directories(../shared)

# Source files
set(PON_SOURCES
    src/pon/notifier.cpp
    src/pon/rule.cpp
    src/pon/premise.cpp
    src/pon/action.cpp
    src/pon/instigation.cpp
)

set(FBE_SOURCES
    src/fbe/fbe_base.cpp
    src/fbe/fbe_tell_storage.cpp
    src/fbe/fbe_extractor.cpp
)

set(IO_SOURCES
    src/io/inotify_watcher.cpp
    src/io/redis_publisher.cpp
    src/io/redis_subscriber.cpp
)

set(EXTRACTOR_SOURCES
    src/extractors/text_extractor.cpp
    src/extractors/pdf_extractor.cpp
    src/extractors/image_extractor.cpp
    src/extractors/audio_extractor.cpp
    src/extractors/office_extractor.cpp
)

set(ALL_SOURCES
    src/main.cpp
    ${PON_SOURCES}
    ${FBE_SOURCES}
    ${IO_SOURCES}
    ${EXTRACTOR_SOURCES}
)

add_executable(kad-tell ${ALL_SOURCES})

# Target Link Libraries
target_link_libraries(kad-tell PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    redis++
    libcurl
    pthread
    # nlohmann_json::nlohmann_json
)
