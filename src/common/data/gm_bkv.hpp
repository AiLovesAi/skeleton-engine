#pragma once

#include "gm_file.hpp"

#include <cstdint>

namespace game {
    class BKV {
        public:
            // Enums
            // Binary Key Value Tags
            // Key/Value format:
            // |===ID===|================KEY============================|===VALUE===|
            // | tagID  | BKV_UINT8 (name length) + UTF-8 String (name) | <Payload> |
            // |========|===============================================|===========|
            // Example: (BKV_COMPOUND + strLen + "Compound"){ (BKV_UINT32 + strLen + "ID") + 0xdeadbeef }BKV_END
            // Example HEX: 0108<0x"Compound"> 0b02<0x"ID">deadbeef 00
            // Example SBKV: {Compound:{ID:0xdeadbeef}}
            // Payloads for each type are listed below:
            enum BKV_Tags {
                BKV_END = 0, // End of compound (does not have a name)
                BKV_COMPOUND, // List of tags, closed by BKV_END
                BKV_LIST, // BKV_UINT8 (tagID) + BKV_UINT32 (size) + Array of BKV_<tagID>
                BKV_UINT8, // 1B Unsigned Int
                BKV_UINT8_ARRAY, // BKV_UINT32 (size) + Array of BKV_UINT8
                BKV_INT8, // 1B Signed Int
                BKV_INT8_ARRAY, // BKV_UINT32 (size) + Array of BKV_INT8
                BKV_UINT16, // 2B Unsigned Int
                BKV_UINT16_ARRAY, // BKV_UINT32 (size) + Array of BKV_UINT16
                BKV_INT16, // 2B Signed Int
                BKV_INT16_ARRAY, // BKV_UINT32 (size) + Array of BKV_INT16
                BKV_UINT32, // 4B Unsigned Int
                BKV_UINT32_ARRAY, // BKV_UINT32 (size) + Array of BKV_UINT32
                BKV_INT32, // 4B Signed Int
                BKV_INT32_ARRAY, // BKV_UINT32 (size) + Array of BKV_INT32
                BKV_UINT64, // 8B Unsigned Int
                BKV_UINT64_ARRAY, // BKV_UINT32 (size) + Array of BKV_UINT64
                BKV_INT64, // 8B Signed Int
                BKV_INT64_ARRAY, // BKV_UINT32 (size) + Array of BKV_INT64
                BKV_FLOAT, // 4B Signed Float
                BKV_FLOAT_ARRAY, // BKV_UINT32 (size) + Array of BKV_FLOAT
                BKV_DOUBLE, // 8B Signed Float
                BKV_DOUBLE_ARRAY, // BKV_UINT32 (size) + Array of BKV_DOUBLE
                BKV_STRING, // BKV_UINT16 (length) + UTF-8 String
                BKV_STRING_ARRAY, // BKV_UINT32 (size) + Array of BKV_STRING
            };
    };
}
