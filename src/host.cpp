#include <common/gm_core.hpp>
#include <common/data/file/gm_logger.hpp>
#include <server/gm_server_instance.hpp>

using namespace game;

int main (int argc, char** argv)
{
    Core::init("../logs/_serverlatest.log", "../logs/_servercrash.txt");
    
    ServerInstance& server = ServerInstance::instance();
    server.start();

    Logger::log(LOG_INFO, "Server closed successfully.");

    return EXIT_SUCCESS;
}
