#include "gm_game.hpp"
#include "util/gm_logger.hpp"
#include "util/gm_file.hpp"

#include <algorithm>
#include <csignal>
#include <ctime>
#include <random>
#include <thread>
#include <cstdlib>

using namespace game;

void signalHandler(int signum) {
    switch (signum) {
        case SIGINT:
            Logger::crash("(SIGINT) Received interrupt signal.");
            break;
        case SIGILL:
            Logger::crash("(SIGILL) Illegal processor instruction.");
            break;
        case SIGFPE:
            Logger::crash("(SIGFPE) Floating point exception caused by overflow/underflow or division by zero.");
            break;
        case SIGSEGV:
            Logger::crash("(SIGSEGV) Attempted to read/write memory whose address was not allocated.");
            break;
        case SIGABRT:
            Logger::crash("(SIGABRT) Abort signal was raised.");
            break;
        case SIGTERM:
            Logger::logMsg(LOG_INFO, "(SIGTERM) Received termination signal. Closing.");
            exit(EXIT_SUCCESS);
            break;
        default:
          Logger::crash("Unknown signal was raised.");
          break;
    }
}

char* getCmdOption(char ** begin, char ** end, const std::string &option)
{
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

void init() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGILL, signalHandler);
    std::signal(SIGSEGV, signalHandler);
    std::signal(SIGFPE, signalHandler);
    std::signal(SIGABRT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    std::srand(std::time(0));
    Game::gameThreads.emplace(std::this_thread::get_id(), "Main");
    File::init();
    Logger::setPaths("logs/latest.log", "logs/crash.txt");
    Logger::logMsg(LOG_INFO, "Initializing game.");
    Logger::logMsg(LOG_INFO, "Using CPU: " + File::CPUStr);
}

int main (int argc, char **argv)
{
    init();

    bool isServer = false;
    // Parse args
    if (cmdOptionExists(argv, argv + argc, "--server")) isServer = true;

    Game game{isServer};

    try {
        game.start();
    } catch (const std::exception &e) {
        Logger::crash(e.what());
        return EXIT_FAILURE;
    }

    Logger::logMsg(LOG_INFO, "Game exited successfully.");

    return EXIT_SUCCESS;
}
