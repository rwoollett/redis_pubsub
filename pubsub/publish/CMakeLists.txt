file(GLOB PUBSUB_PUBLISH_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(RedisPublish STATIC
    ${PUBSUB_PUBLISH_SOURCES}
)

add_library(RedisPubSub::RedisPublish ALIAS RedisPublish)

target_include_directories(RedisPublish
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/redis_pubsub/publish>
)

target_link_libraries(RedisPublish
    PUBLIC
        MTLog::MTLog
)

if (WIN32)
    find_package(OpenSSL REQUIRED)
    target_link_libraries(RedisPublish
        PRIVATE
            OpenSSL::SSL
            OpenSSL::Crypto
            ws2_32
    )
endif()

install(TARGETS RedisPublish
    EXPORT RedisPubSubTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/redis_pubsub/publish
    FILES_MATCHING PATTERN "*.h"
)
