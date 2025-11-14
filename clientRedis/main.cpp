#include "io_utility/io_utility.h"
#include <csignal>
#include <cstdlib> // For std::getenv
#include "../redisSubscribe/Subscribe.h"
#include "AwakenerWaitable.h"
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <boost/redis/connection.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/redis/src.hpp> // boost redis implementation

int main(int argc, char **argv)
{

  // Check all environment variable
  const char *redis_host = std::getenv("REDIS_HOST");
  const char *redis_port = std::getenv("REDIS_PORT");
  const char *redis_channel = std::getenv("REDIS_CHANNEL");
  const char *redis_password = std::getenv("REDIS_PASSWORD");

  if (!(redis_host && redis_port && redis_password && redis_channel))
  {
    std::cerr << "Environment variables REDIS_HOST, REDIS_PORT, REDIS_CHANNEL or REDIS_PASSWORD are not set." << std::endl;
    exit(1);
  }

  boost::asio::io_context main_ioc;
  boost::asio::signal_set sig_set(main_ioc.get_executor(), SIGINT, SIGTERM);
  AwakenerWaitable awakener;
  bool m_worker_shall_stop{false}; // false

#if defined(SIGQUIT)
  sig_set.add(SIGQUIT);
#endif // defined(SIGQUIT)

  std::cout << "co_main wait to signal" << std::endl;
  sig_set.async_wait(
      [&](const boost::system::error_code &, int)
      {
        m_worker_shall_stop = 1;
        awakener.stop();
      });

  auto main_ioc_thread = std::thread([&main_ioc]()
                                     { main_ioc.run(); });

  try
  {
    RedisSubscribe::Subscribe redisSubscribe;
    redisSubscribe.main_redis(awakener);

    std::cout << "Application loop stated\n";
    while (!m_worker_shall_stop)
    {
      awakener.wait_broadcast();
      std::cout << "Application loop awakened, awake count: " << awakener.awake_load() << std::endl;

      if (redisSubscribe.isSignalStopped())
      {
        std::cout << "Signal to Stopped" << std::endl;
        m_worker_shall_stop = true;
      }
    }

    main_ioc.stop();
    if (main_ioc_thread.joinable())
    {
      main_ioc_thread.join();
    }
  }
  catch (const std::exception &e)
  {
    std::cout << e.what() << "\n";
    return EXIT_FAILURE;
  }
  catch (const std::string &e)
  {
    std::cout << e << "\n";
    return EXIT_FAILURE;
  }

  std::cout << "Exited normally\n";
  return EXIT_SUCCESS;
}
