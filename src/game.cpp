#include <common/gm_core.hpp>
#include <common/data/file/gm_logger.hpp>
#include <client/gm_client.hpp>

#include <cstdlib>

using namespace game;

#include <sstream>
#include <memory>
#include <common/data/file/gm_file.hpp>
#include <common/data/bkv/gm_bkv.hpp>
#include <common/data/bkv/gm_sbkv.hpp>
void test() {
    try {
        UTF8Str test = FormatString::formatString("%lu", sizeof(float128_t));
        Logger::log(LOG_INFO, test);
        uint64_t a = FormatString::strToUInt<uint64_t>("ffffffffffffffff", 16);
        UTF8Str a1 = FormatString::formatString("%lx", a);
        Logger::log(LOG_INFO, a1);
        int32_t b = FormatString::strToInt<int32_t>("-69696969");
        UTF8Str b1 = FormatString::formatString("%d", b);
        Logger::log(LOG_INFO, b1);
        float32_t c = FormatString::strToFloat<float32_t>("-0.00012345");
        UTF8Str c1 = FormatString::formatString("%f", c);
        Logger::log(LOG_INFO, c1);
        float128_t d = FormatString::strToFloat<float128_t>("-0000.0000000000000000694206942069420694206942069420314159265358972");
        UTF8Str d1 = FormatString::formatString("%.18lf", d);
        Logger::log(LOG_INFO, d1);
        bool e = FormatString::strToBool("TRUE");
        UTF8Str e1 = FormatString::formatString("%B", e);
        Logger::log(LOG_INFO, e1);
        float64_t f = FormatString::strToFloat<float64_t>("1234567890.12345678901");
        UTF8Str f1 = FormatString::formatString("%.18f", f);
        Logger::log(LOG_INFO, f1);
        float32_t g = FormatString::strToFloat<float32_t>("654321.789");
        UTF8Str g1 = FormatString::formatString("%.18f", g);
        Logger::log(LOG_INFO, g1);
        UTF8Str h1 = FormatString::formatString("%.18lf", LDBL_MAX);
        Logger::log(LOG_INFO, h1);
        UTF8Str i1 = FormatString::formatString("%.18lf", LDBL_MIN);
        Logger::log(LOG_INFO, i1);
        UTF8Str j1 = FormatString::formatString("%.18lf", static_cast<float128_t>(INFINITY));
        Logger::log(LOG_INFO, j1);
        UTF8Str k1 = FormatString::formatString("%.18lf", static_cast<float128_t>(NAN));
        Logger::log(LOG_INFO, k1);
    } catch (std::runtime_error& e) { Logger::crash(e.what()); }

    char8_t data[] = u8"{ Test:{ id:-127., str: hi,\"☺☺☺\":-69, '☺☺☺':ok, names:[\"Tyrone\", 'dodo', '錯世☺', Steve] soul:{ personality:crappy-go-fucky, lifespan:'3 gays to live', alive:True }, bills_In_Wallet: [10b,20b,1b,1b,10b, 5b], funnyNumber:42069L }, TotalCost:100.2, MiniCost: -100.1f, unsigned:[5us, 6969US, 2uS, 65535Us], Finish:el_fin }";
    char *buffer = static_cast<char*>(std::malloc(sizeof(data)));
    std::memcpy(buffer, data, sizeof(data));
    std::stringstream msg;
    msg << "SBKV: " << buffer;
    Logger::log(LOG_INFO, msg.str().c_str());
    
    UTF8Str stringified{sizeof(data), std::shared_ptr<const char>(buffer, std::free)};
    try {
        BKV_t bkv = BKV::bkvFromSBKV(stringified);
        Logger::log(LOG_INFO, "Parsed BKV.");
        File::FileContents contents{static_cast<size_t>(bkv.size()), bkv.data()};
        File::writeFile("bkv.txt", contents);

        UTF8Str sbkv = SBKV::sbkvFromBKV(bkv);
        Logger::log(LOG_INFO, "Parsed SBKV.");
        msg.str("");
        msg << sbkv.get();
        Logger::log(LOG_INFO, msg.str().c_str());
        Logger::log(LOG_INFO, "Complete.");
        
        bkv = BKV::bkvFromSBKV(stringified);
        Logger::log(LOG_INFO, "Reparsed BKV.");
        File::FileContents contents2{static_cast<size_t>(bkv.size()), bkv.data()};
        File::writeFile("bkv2.txt", contents2);
        
        sbkv = SBKV::sbkvFromBKV(bkv);
        Logger::log(LOG_INFO, "Reparsed SBKV.");
        msg.str("");
        msg << sbkv.get();
        Logger::log(LOG_INFO, msg.str().c_str());
    } catch (std::runtime_error& e) { Logger::crash(e.what()); }
    
}

int main (int argc, char** argv)
{
    Core::init("../logs/latest.log", "../logs/crash.txt");
    
    test();
    Client::init();
    Client& client = Client::instance();
    client.start();

    Logger::log(LOG_INFO, "Game exited successfully.");

    return EXIT_SUCCESS;
}
