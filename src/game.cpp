#include <common/gm_core.hpp>
#include <common/data/file/gm_logger.hpp>
#include <client/gm_client.hpp>

#include <cstdlib>

using namespace game;

/*
#include <sstream>
#include <memory>
#include <common/data/file/gm_file.hpp>
#include <common/data/bkv/gm_bkv.hpp>
#include <common/data/bkv/gm_sbkv.hpp>
void test() {
    char8_t data[] = u8"{ Test:{ id:-127., str: hi,\"☺☺☺\":-69, '☺☺☺':ok, names:[\"Tyrone\", 'dodo', '錯世☺', Steve] soul:{ personality:crappy-go-fucky, lifespan:'3 gays to live', alive:True }, bills_In_Wallet: [10b,20b,1b,1b,10b, 5b], funnyNumber:42069L }, TotalCost:100.2, MiniCost: -100.1f, unsigned:[5us, 6969US, 2uS, 65535Us], Finish:el_fin }";
    char *buffer = static_cast<char*>(std::malloc(sizeof(data)));
    std::memcpy(buffer, data, sizeof(data));
    std::stringstream msg;
    msg << "SBKV: " << buffer;
    Logger::log(LOG_INFO, msg.str());
    
    UTF8Str stringified{sizeof(data), std::shared_ptr<const char>(buffer, std::free)};
    BKV_t bkv;
    try {
        bkv = BKV::bkvFromSBKV(stringified);
    } catch (std::runtime_error& e) { Logger::crash(e.what()); }
    Logger::log(LOG_INFO, "Parsed BKV.");
    File::FileContents contents{static_cast<size_t>(bkv.size()), bkv.data};
    File::writeFile("bkv.txt", contents);

    UTF8Str sbkv;
    try {
        sbkv = SBKV::sbkvFromBKV(bkv);
    } catch (std::runtime_error& e) { Logger::crash(e.what()); }
    Logger::log(LOG_INFO, "Parsed SBKV.");
    msg.str("");
    msg << sbkv.get();
    Logger::log(LOG_INFO, msg.str());
    Logger::log(LOG_INFO, "Complete.");
    
    try {
        bkv = BKV::bkvFromSBKV(stringified);
    } catch (std::runtime_error& e) { Logger::crash(e.what()); }
    Logger::log(LOG_INFO, "Reparsed BKV.");
    contents.length() = static_cast<size_t>(bkv.size());
    contents.data = bkv.get();
    File::writeFile("bkv2.txt", contents);
    
    try {
        sbkv = SBKV::sbkvFromBKV(bkv);
    } catch (std::runtime_error& e) { Logger::crash(e.what()); }
    Logger::log(LOG_INFO, "Reparsed SBKV.");
    msg.str("");
    msg << sbkv.get();
    Logger::log(LOG_INFO, msg.str());
    
}*/

int main (int argc, char** argv)
{
    Core::init("../logs/latest.log", "../logs/crash.txt");
    
    //test();
    Client::init();
    Client& client = Client::instance();
    client.start();

    Logger::log(LOG_INFO, "Game exited successfully.");

    return EXIT_SUCCESS;
}
