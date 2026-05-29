file(GLOB PUBSUB_SUBSCRIBE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(RedisSubscribe STATIC
    ${PUBSUB_SUBSCRIBE_SOURCES}
)

add_library(RedisPubSub::RedisSubscribe ALIAS RedisSubscribe)

target_include_directories(RedisSubscribe
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/redis_pubsub/subscribe>
)

target_link_libraries(RedisSubscribe
    PUBLIC
        MTLog::MTLog
)

install(TARGETS RedisSubscribe
    EXPORT RedisPubSubTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/redis_pubsub/subscribe
    FILES_MATCHING PATTERN "*.h"
)
