#include <core/gm_core.hpp>
#include <core/logger/gm_logger.hpp>
#include <server/gm_server.hpp>

using namespace game;

int main (int argc, char** argv)
{
    Core::init("../logs/server_latest.log", "../logs/server_crash.txt");
    
    Server& server = Server::instance();
    server.start();

    Logger::log(LOG_INFO, "Server closed successfully.");

    return EXIT_SUCCESS;
}