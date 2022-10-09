#include "gm_bkv.hpp"

namespace game {
    template <> const uint8_t BKV::BKVTypeMap<bool>::tagID = BKV_UI8;
    template <> const uint8_t BKV::BKVTypeMap<uint8_t>::tagID = BKV_UI8;
    template <> const uint8_t BKV::BKVTypeMap<int8_t>::tagID = BKV_I8;
    template <> const uint8_t BKV::BKVTypeMap<uint16_t>::tagID = BKV_UI16;
    template <> const uint8_t BKV::BKVTypeMap<int16_t>::tagID = BKV_I16;
    template <> const uint8_t BKV::BKVTypeMap<uint32_t>::tagID = BKV_UI32;
    template <> const uint8_t BKV::BKVTypeMap<int32_t>::tagID = BKV_I32;
    template <> const uint8_t BKV::BKVTypeMap<uint64_t>::tagID = BKV_UI64;
    template <> const uint8_t BKV::BKVTypeMap<int64_t>::tagID = BKV_I64;
}