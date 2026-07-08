
#include <csignal>
#include <cstdlib> // For std::getenv
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include "../pubsub/publish/Publish.h" // RedisPublish class
#include <boost/redis/src.hpp>         // boost redis implementation
#include <mtlog/mt_log.hpp>
#if defined(_WIN32)
    #include <conio.h>
    // Windows console handling
#else
    #include <termios.h>
    #include <unistd.h>
    // POSIX console handling
#endif
//#include <unistd.h>

char read_getch()
{
#if defined(_WIN32)
    return _getch();
#else
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    char c = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return c;
#endif
}

int main(int argc, char **argv)
{

  // Check all environment variable
  const char *redis_host = std::getenv("REDIS_HOST");
  const char *redis_port = std::getenv("REDIS_PORT");
  const char *redis_channel = std::getenv("REDIS_CHANNEL");
  const char *redis_gateway_channel = std::getenv("REDIS_GATEWAY_CHANNEL");
  const char *redis_password = std::getenv("REDIS_PASSWORD");
  const char *MTLOG_LOGFILE = std::getenv("MTLOG_LOGFILE");

  if (!(redis_host && redis_port && redis_password && redis_channel && redis_gateway_channel))
  {
    std::cerr << "Environment variables REDIS_HOST, REDIS_PORT, REDIS_CHANNEL, REDIS_GATEWAY_CHANNEL, REDIS_PASSWORD or REDIS_USE_SSL are not set." << std::endl;
    exit(1);
  }
  if (argc > 1)
  {
    std::cerr << "Using command line arguments as channels to publish messages." << std::endl;
  }

  mt_logging::logger().log({MTLOG_LOGFILE, mt_logging::LogLevel::Error, true});

  try
  {

    RedisPublish::Publish redisPublish;
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    auto doPublish = [&redisPublish](const std::string &channel,
                                     const std::string &msg = "default message")
    {
      if (!redisPublish.is_redis_signaled())
      {
        redisPublish.dispatch_message(channel, msg);
      }
      else
      {
        std::cerr << "doPublish found redis disconnection\n";
      }
    };

    bool m_worker_shall_stop{false}; // false
    while (!m_worker_shall_stop)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));

      if (redisPublish.is_redis_signaled())
      {
        m_worker_shall_stop = true;
        break;
      }

      std::cout << "Press any key to publish..." << std::endl;
      char key = read_getch();
      if (argc > 1)
      {
        for (int i = 1; i < argc; ++i)
        {
          doPublish(argv[i]);
        }
      }
      else
      {
        doPublish("ws_events");
        doPublish("ttt_player_Move");
      }
    }
  }
  catch (const std::exception &e)
  {
    mt_logging::logger().log(
        {fmt::format("Application error {}", e.what()),
         mt_logging::LogLevel::Error,
         true});
    return EXIT_FAILURE;
  }
  catch (const std::string &e)
  {
    mt_logging::logger().log(
        {fmt::format("Application error {}", e),
         mt_logging::LogLevel::Error,
         true});
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
