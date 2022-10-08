#pragma once

#include <cstdint>

namespace game {
    class BKV {
        public:
            // Enums
            // Binary Key Value Tags
            // Key/Value format:
            // |===ID===|================KEY==========================|===VALUE===|
            // | tagID  | BKV_UI8 (name length) + UTF-8 String (name) | <Payload> |
            // |========|=============================================|===========|
            // Example: (BKV_COMPOUND + strLen + "Compound"){ (BKV_UI32 + strLen + "id") + 0xdeadbeef }BKV_END
            // Example HEX: 0108<0x"Compound"> 0b02<0x"id">deadbeef 00
            // Example SBKV: {Compound:{id:0xdeadbeef}}
            // Payloads for each type are listed below:
            enum BKV_Tags {
                BKV_END, // End of compound (does not have a name)
                BKV_COMPOUND, // List of tags, closed by BKV_END
                BKV_LIST, // BKV_UI8 (tagID) + BKV_UI32 (size) + Array of BKV_<tagID>
                BKV_UI8, // 1B Unsigned Int
                BKV_UI8_ARRAY, // BKV_UI32 (size) + Array of BKV_UI8
                BKV_I8, // 1B Signed Int
                BKV_I8_ARRAY, // BKV_UI32 (size) + Array of BKV_I8
                BKV_UI16, // 2B Unsigned Int
                BKV_UI16_ARRAY, // BKV_UI32 (size) + Array of BKV_UI16
                BKV_I16, // 2B Signed Int
                BKV_I16_ARRAY, // BKV_UI32 (size) + Array of BKV_I16
                BKV_UI32, // 4B Unsigned Int
                BKV_UI32_ARRAY, // BKV_UI32 (size) + Array of BKV_UI32
                BKV_I32, // 4B Signed Int
                BKV_I32_ARRAY, // BKV_UI32 (size) + Array of BKV_I32
                BKV_UI64, // 8B Unsigned Int
                BKV_UI64_ARRAY, // BKV_UI32 (size) + Array of BKV_UI64
                BKV_I64, // 8B Signed Int
                BKV_I64_ARRAY, // BKV_UI32 (size) + Array of BKV_I64
                BKV_FLOAT, // 4B Signed Float
                BKV_FLOAT_ARRAY, // BKV_UI32 (size) + Array of BKV_FLOAT
                BKV_DOUBLE, // 8B Signed Float
                BKV_DOUBLE_ARRAY, // BKV_UI32 (size) + Array of BKV_DOUBLE
                BKV_STR, // BKV_UI16 (length) + UTF-8 String
                BKV_STR_ARRAY, // BKV_UI32 (size) + Array of BKV_STR
            };
    };
}
