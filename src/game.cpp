#include <common/gm_core.hpp>
#include <common/data/gm_logger.hpp>
#include <client/gm_client.hpp>

#include <cstdlib>

using namespace game;

#include <common/data/bkv/gm_bkv.hpp>
#include <common/data/bkv/gm_sbkv.hpp>
#include <common/data/gm_logger.hpp>

int main (int argc, char** argv)
{
    Core::init("../logs/latest.log", "../logs/crash.txt");

    char str[] = "{Test:{id:1024, personality:'3 fucked', alive:trUE}, t3i:[1.,-5.2,100.000]}";
    char *buffer = static_cast<char*>(std::malloc(sizeof(str)));
    std::memcpy(buffer, str, sizeof(str));
    Logger::log(LOG_INFO, buffer);

    UTF8Str start = {sizeof(buffer), std::shared_ptr<char>(buffer, std::free)};
    BKV_t bkv = BKV::bkvFromSBKV(start);
    Logger::log(LOG_INFO, "BKV Complete");
    UTF8Str sbkv = SBKV::sbkvFromBKV(bkv);
    Logger::log(LOG_INFO, "SBKV Complete");
    Logger::log(LOG_INFO, sbkv.str.get());
    
    Client::init();
    Client& client = Client::instance();
    client.start();

    Logger::log(LOG_INFO, "Game exited successfully.");

    return EXIT_SUCCESS;
}
