#include <common/gm_core.hpp>
#include <common/data/gm_logger.hpp>
#include <client/gm_client.hpp>

#include <cstdlib>

using namespace game;


#include <sstream>
#include <memory>
#include <common/data/gm_file.hpp>
#include <common/data/bkv/gm_bkv.hpp>
void test() {
    char sbkv[] = "{ Test:{ id:1024, names:[\"Tyrone\", 'Momo', Steve], soul:{ personality:happy-go-lucky, lifespan:'3 days to live', alive:True }, bills_In_Wallet: [10b,20b,1b,1b,10b, 5b], funnyNumber:42069L }, TotalCost:100.2, MiniCost: 100.2f }";
    char *buffer = static_cast<char*>(std::malloc(sizeof(sbkv)));
    std::memcpy(buffer, sbkv, sizeof(sbkv));
    std::stringstream msg;
    msg << "SBKV: " << buffer;
    Logger::log(LOG_INFO, msg.str());
    
    BKV::UTF8Str stringified{sizeof(sbkv), std::shared_ptr<char>(buffer, std::free)};
    BKV bkv{stringified};
    Logger::log(LOG_INFO, "Complete.");
}

int main (int argc, char** argv)
{
    Core::init("../logs/latest.log", "../logs/crash.txt");
    
    test();
    /*Client::init();
    Client& client = Client::instance();
    client.start();*/

    Logger::log(LOG_INFO, "Game exited successfully.");

    return EXIT_SUCCESS;
}
