#include <common/gm_core.hpp>
#include <common/logger/gm_logger.hpp>
#include <server/gm_server_instance.hpp>

using namespace game;

int main (int argc, char** argv)
{
    Core::init("../logs/server_latest.log", "../logs/server_crash.txt");
    
    ServerInstance& server = ServerInstance::instance();
    server.start();

    Logger::log(LOG_INFO, "Server closed successfully.");

    return EXIT_SUCCESS;
}
