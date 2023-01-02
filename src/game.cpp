#include <common/gm_core.hpp>
#include <common/data/gm_logger.hpp>
#include <client/gm_client.hpp>

#include <cstdlib>

using namespace game;

int main (int argc, char** argv)
{
    Core::init("../logs/latest.log", "../logs/crash.txt");
    
    Client::init();
    Client& client = Client::instance();
    client.start();

    Logger::log(LOG_INFO, "Game exited successfully.");

    return EXIT_SUCCESS;
}
