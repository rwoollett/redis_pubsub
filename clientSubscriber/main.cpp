#include <csignal>
#include <cstdlib> // For std::getenv
#include "../pubsub/subscribe/Subscribe.h"
#include "AwakenerWaitable.h"
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <boost/redis/connection.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/redis/src.hpp> // boost redis implementation
#include <mtlog/mt_log.hpp>
#include <string>
#include <stdexcept>

int main(int argc, char **argv)
{
  int result = EXIT_SUCCESS;
  const char *redis_host = std::getenv("REDIS_HOST");
  const char *redis_port = std::getenv("REDIS_PORT");
  const char *redis_channel = std::getenv("REDIS_CHANNEL");
  const char *redis_password = std::getenv("REDIS_PASSWORD");
  const char *redis_use_ssl = std::getenv("REDIS_USE_SSL");
  const char *MTLOG_LOGFILE = std::getenv("MTLOG_LOGFILE");

  if (!(redis_host && redis_port && redis_password && redis_channel))
  {
    std::cerr << "Environment variables REDIS_HOST, REDIS_PORT, REDIS_CHANNEL, REDIS_PASSWORD or REDIS_USE_SSL are not set." << std::endl;
    exit(1);
  }

  mt_logging::logger().log({MTLOG_LOGFILE, true});

  boost::asio::io_context main_ioc;
  AwakenerWaitable awakener;
  bool m_worker_shall_stop{false};
  auto main_ioc_thread = std::thread([&main_ioc]()
                                     { main_ioc.run(); });

  try
  {
    RedisSubscribe::Subscribe redisSubscribe(awakener);
    redisSubscribe.main_redis();

    mt_logging::logger().log(
        {"Application loop stated",
         true});

    while (!m_worker_shall_stop)
    {
      awakener.wait_broadcast();
      mt_logging::logger().log(
          {fmt::format("Application loop awakened, awake count: {} ", awakener.awake_load()),
           true});

      if (redisSubscribe.is_signal_stopped())
      {
        m_worker_shall_stop = true;
      }
    }

    mt_logging::logger().log(
        {"Exited normally",
         true});
  }
  catch (const std::exception &e)
  {
    mt_logging::logger().log(
        {fmt::format("Application error {} ", e.what()),
         true});
    result = EXIT_FAILURE;
  }
  catch (const std::string &e)
  {
    mt_logging::logger().log(
        {fmt::format("Application error {} ", e),
         true});
    result = EXIT_FAILURE;
  }

  main_ioc.stop();
  if (main_ioc_thread.joinable())
  {
    main_ioc_thread.join();
  }

  return result;
}
