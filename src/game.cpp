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
#include <common/data/bkv/gm_bkv_builder.hpp>
void test() {
    // FormatString/strToInt test
    try {
        UTF8Str test = FormatString::formatString("%lu", sizeof(float128_t));
        Logger::log(LOG_INFO, test);
        uint64_t a = FormatString::strToInt<uint64_t>("ffffffffffffffff", 16);
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

    // SBKV -> BKV -> SBKV test
    char8_t data[] = u8"{ Test:{ id:-127., str: hi,\"☺☺☺\":-69, '☺☺☺':ok, names:[\"Tyrone\", 'dodo', '錯世☺', Steve] soul:{ personality:crappy-go-fucky, lifespan:'3 gays to live', alive:True }, bills_In_Wallet: [10b,20b,1b,1b,10b, 5b], funnyNumber:42069L }, TotalCost:100.2, MiniCost: -100.1f, unsigned:[5us, 6969US, 2uS, 65535Us], Finish:el_fin }";
    char *buffer = static_cast<char*>(std::malloc(sizeof(data)));
    std::memcpy(buffer, data, sizeof(data));
    std::stringstream msg;
    msg << "SBKV: " << buffer;
    Logger::log(LOG_INFO, msg.str().c_str());
    
    UTF8Str stringified{sizeof(data), std::shared_ptr<const char>(buffer, std::free)};
    try {
        BKV bkv = BKV::bkvFromSBKV(stringified);
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
        msg.str("");
    } catch (std::runtime_error& e) { Logger::crash(e.what()); }
    
    // BKV_Builder test
    try {
        float128_t nums[] = {0.l, 1.l, 3.14159265l, -0.0006942069};
        UTF8Str strs[3];
        strs[0] = UTF8Str{"test1"};
        strs[1] = UTF8Str{"test2.0"};
        strs[2] = UTF8Str{"test3.1.0"};
        BKV_Builder builder;
        builder.openCompound(UTF8Str("Example1"))
            .setString("name", "sam")
            .setValue<uint8_t>("age", 20)
            .setBool("alive", true)
            .setValueArray<float128_t>("nums", nums, (sizeof(nums) / sizeof(float128_t)))
            .setStringArray("strs", strs, (sizeof(strs) / sizeof(UTF8Str)))
        .closeCompound();
        Logger::log(LOG_INFO, "Completed building BKV.");
        BKV bkv = builder.build();
        Logger::log(LOG_INFO, "Copied BKV.");
        BKV bkv2 = BKV::bkvFromSBKV(UTF8Str("{Example1:{name:'sam',age:20,alive:True,nums:[0.0,1.0,3.14159265,-0.0006942069],'strs':[test1, test2,test3]}}"));
        File::FileContents contents{static_cast<size_t>(bkv.size()), bkv.data()};
        File::writeFile("built-bkv.txt", contents);
        File::FileContents contents2{static_cast<size_t>(bkv2.size()), bkv2.data()};
        File::writeFile("built-bkv-compare.txt", contents2);
        UTF8Str sbkv = SBKV::sbkvFromBKV(bkv);
        msg << "Built SBKV: " << sbkv.get();
        Logger::log(LOG_INFO, msg.str().c_str());
    } catch (std::runtime_error& e) { Logger::crash(e.what()); }
}

int main (int argc, char** argv)
{
    Core::init(UTF8Str{"../logs/latest.log"}, UTF8Str{"../logs/crash.txt"});
    
    test();
    Client::init();
    Client& client = Client::instance();
    client.start();

    Logger::log(LOG_INFO, "Game exited successfully.");

    return EXIT_SUCCESS;
}
