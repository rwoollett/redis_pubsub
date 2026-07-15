cmake_minimum_required(VERSION 3.13)
project(RedisPubSub VERSION 1.0
        DESCRIPTION "Redis on network"
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (WIN32)
  cmake_policy(SET CMP0167 NEW)
endif()

add_compile_definitions(HAVE_ASIO)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_DEBUG_LIBS OFF)
set(Boost_USE_RELEASE_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON) 

find_package(Boost 1.86 REQUIRED)
find_package(fmt REQUIRED)
find_package(MTLog REQUIRED)

option(BUILD_PUBSUB_TESTS "Build tests and include clientProducer and clientSubscriber" OFF)

if (BUILD_PUBSUB_TESTS)
add_subdirectory(clientSubscriber)
add_subdirectory(clientPublisher)
endif()

add_subdirectory(pubsub/publish)
add_subdirectory(pubsub/subscribe)

# -----------------------------
# PACKAGE CONFIG EXPORT
# -----------------------------
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/RedisPubSubConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/RedisPubSubConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/RedisPubSubConfig.cmake"
    INSTALL_DESTINATION lib/cmake/RedisPubSub
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/RedisPubSubConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/RedisPubSubConfigVersion.cmake"
    DESTINATION lib/cmake/RedisPubSub
)

install(EXPORT RedisPubSubTargets
    FILE RedisPubSubTargets.cmake
    NAMESPACE RedisPubSub::
    DESTINATION lib/cmake/RedisPubSub
)



