#include <util/gm_core.hpp>
#include <util/logger/gm_logger.hpp>
#include <server/gm_server.hpp>

using namespace game;

int main (int argc, char** argv)
{
    Core::init();
    
    Server& server = Server::instance();
    server.start();

    Logger::log(LOG_INFO, "Server closed successfully.");

    return EXIT_SUCCESS;
}
