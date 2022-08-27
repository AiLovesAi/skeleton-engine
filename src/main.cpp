#include "gm_game.hpp"
#include "util/gm_logger.hpp"

#include <csignal>
#include <cstdlib>

using namespace game;

int main (int argc, char** argv)
{
    Game game;

    try {
        game.init(argc, argv);
        game.start();
    } catch (const std::exception &e) {
        Logger::crash(e.what());
        return EXIT_FAILURE;
    }

    Logger::logMsg(LOG_INFO, "Game exited successfully.");

    return EXIT_SUCCESS;
}
