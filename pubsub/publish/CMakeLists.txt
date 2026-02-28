
add_library(RedisPublish
  Publish.cpp
  Publish.h
  logsync.cpp
  logsync.h
)

add_library(RedisPubSub::RedisPublish ALIAS RedisPublish)

target_include_directories(RedisPublish 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../..      # <pubsub/...>
    PRIVATE
        $<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(RedisPublish PRIVATE Threads::Threads ${CMAKE_DL_LIBS})


