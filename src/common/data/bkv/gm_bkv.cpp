#include "gm_bkv.hpp"

#include "gm_bkv_parser.hpp"
#include "../string/gm_string.hpp"
#include "../gm_endianness.hpp"

#include "../file/gm_logger.hpp"
#include "../file/gm_file.hpp"

#include <cstring>
#include <sstream>
#include <stdexcept>
#include <type_traits>

namespace game {
    template <> const uint8_t BKV::BKVTypeMap<bool>::tagID = BKV::BKV_UI8;
    template <> const uint8_t BKV::BKVTypeMap<uint8_t>::tagID = BKV::BKV_UI8;
    template <> const uint8_t BKV::BKVTypeMap<int8_t>::tagID = BKV::BKV_I8;
    template <> const uint8_t BKV::BKVTypeMap<uint16_t>::tagID = BKV::BKV_UI16;
    template <> const uint8_t BKV::BKVTypeMap<int16_t>::tagID = BKV::BKV_I16;
    template <> const uint8_t BKV::BKVTypeMap<uint32_t>::tagID = BKV::BKV_UI32;
    template <> const uint8_t BKV::BKVTypeMap<int32_t>::tagID = BKV::BKV_I32;
    template <> const uint8_t BKV::BKVTypeMap<uint64_t>::tagID = BKV::BKV_UI64;
    template <> const uint8_t BKV::BKVTypeMap<int64_t>::tagID = BKV::BKV_I64;
    template <> const uint8_t BKV::BKVTypeMap<float>::tagID = BKV::BKV_FLOAT;
    template <> const uint8_t BKV::BKVTypeMap<double>::tagID = BKV::BKV_DOUBLE;

    BKV_t BKV::bkvFromSBKV(const UTF8Str& stringified) {
        const char* sbkv = stringified.get();
        Logger::log(LOG_INFO, "Reached bkvFromSBKV.");
        BKV_Parser buf;
        Logger::log(LOG_INFO, "Parsing...");
        for (int64_t i = 0; i < stringified.length(); i++) {
        std::stringstream m;
        m << "Parsing character: " << i;
        Logger::logSync(LOG_INFO, m.str(), std::this_thread::get_id());
            try { buf.state()->parse(buf, sbkv[i]); } catch (std::runtime_error &e) { throw; }
        }
        Logger::log(LOG_INFO, "Parsing complete.");
        return BKV_t{buf.size(), buf.data()};
    }
}