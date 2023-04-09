#pragma once

#include "states/gm_bkv_parser_state.hpp"
#include "../string/gm_utf8.hpp"

#include <cstdint>
#include <memory>

namespace game {
    class BKV {
        public:
            // Types
            template <typename T>
            struct BKVTypeMap { static const uint8_t tagID; };
            
            // Binary Key Value Tags
            // Key/Value format:
            // |===ID===|==================KEY==================|===VALUE===|
            // | tagID  | BKV_UI8 (name length) + String (name) | <Payload> |
            // |========|=======================================|===========|
            // Example: (BKV_COMPOUND + strLen + "Compound"){ (BKV_UI32 + strLen + "id") + 0xdeadbeef }BKV_END
            // Example BKV in HEX: 0108<0x"Compound"> 8402<0x"id">deadbeef 00    (19 bytes)
            // Example SBKV: {Compound:{id:-559038737}}                          (26 bytes)
            // Payloads for each type are listed below:
            enum BKV_Flags {
                BKV_UNSIGNED = 1 << 7, // Specified tag is unsigned
                BKV_ARRAY = 1 << 6, // Specified tag is an array
                
                // WARNING: Updating the number of flags constrains how many data types there can be.
                // With two flags, there can be 2^6 (64) data types. Each extra flag halves this.
                BKV_FLAGS_ALL = BKV_UNSIGNED | BKV_ARRAY,
            };
            enum BKV_Tags {
                // Data types:
                BKV_END, // End of compound (does not have a key)
                BKV_COMPOUND, // Group of tags, closed by BKV_END - BKV_UI32 (length including BKV_END)
                BKV_BOOL, // 1B Unsigned Int (0 or 1)
                BKV_I8, // 1B Signed Int
                BKV_I16, // 2B Signed Int
                BKV_I32, // 4B Signed Int
                BKV_I64, // 8B Signed Int
                BKV_FLOAT, // 4B Signed Float
                BKV_DOUBLE, // 8B Signed Float
                BKV_STR, // BKV_UI16 (length) + UTF-8 String

                // Combined types (for quick access):
                BKV_I8_ARRAY = BKV_I8 | BKV_ARRAY, // BKV_UI16 (size) + Array of BKV_I8
                BKV_UI8 = BKV_I8 | BKV_UNSIGNED, // 1B Unsigned Int
                BKV_UI8_ARRAY = BKV_UI8 | BKV_ARRAY, // BKV_UI16 (size) + Array of BKV_UI8

                BKV_I16_ARRAY = BKV_I16 | BKV_ARRAY, // BKV_UI16 (size) + Array of BKV_I16
                BKV_UI16 = BKV_I16 | BKV_UNSIGNED, // 2B Unsigned Int
                BKV_UI16_ARRAY = BKV_UI16 | BKV_ARRAY, // BKV_UI16 (size) + Array of BKV_UI16

                BKV_I32_ARRAY = BKV_I32 | BKV_ARRAY, // BKV_UI16 (size) + Array of BKV_I32
                BKV_UI32 = BKV_I32 | BKV_UNSIGNED, // 4B Unsigned Int
                BKV_UI32_ARRAY = BKV_UI32 | BKV_ARRAY, // BKV_UI16 (size) + Array of BKV_UI32

                BKV_I64_ARRAY = BKV_I64 | BKV_ARRAY, // BKV_UI16 (size) + Array of BKV_I64
                BKV_UI64 = BKV_I64 | BKV_UNSIGNED, // 8B Unsigned Int
                BKV_UI64_ARRAY = BKV_UI64 | BKV_ARRAY, // BKV_UI16 (size) + Array of BKV_UI64

                BKV_FLOAT_ARRAY = BKV_FLOAT | BKV_ARRAY, // BKV_UI16 (size) + Array of BKV_FLOAT
                BKV_DOUBLE_ARRAY = BKV_DOUBLE | BKV_ARRAY, // BKV_UI16 (size) + Array of BKV_DOUBLE
                BKV_STR_ARRAY = BKV_STR | BKV_ARRAY, // BKV_UI16 (size) + Array of BKV_STR
            };

            enum BKV_Limits {
                BKV_COMPOUND_SIZE = sizeof(uint32_t),
                BKV_COMPOUND_MAX = UINT32_MAX,
                BKV_COMPOUND_DEPTH_MAX = UINT8_MAX,
                BKV_KEY_SIZE = sizeof(uint8_t),
                BKV_KEY_MAX = UINT8_MAX,
                BKV_ARRAY_SIZE = sizeof(uint16_t),
                BKV_ARRAY_MAX = UINT16_MAX,
                BKV_STR_SIZE = sizeof(uint16_t),
                BKV_STR_MAX = UINT16_MAX,
            };

            // Constructors
            BKV(int64_t size, std::shared_ptr<const uint8_t> data) : _size{size}, _data{data} {}
            ~BKV() {}
            
            BKV& operator=(const BKV& bkv) {
                _size = bkv.size();
                uint8_t* copy = static_cast<uint8_t*>(std::malloc(_size));
                std::memcpy(copy, bkv.get(), _size);
                _data = std::shared_ptr<const uint8_t>(copy, std::free);
                return *this;
            }
            
            // Functions
            static BKV bkvFromSBKV(const UTF8Str& stringified);
            
            int64_t size() const { return _size; }
            std::shared_ptr<const uint8_t> data() const { return _data; }
            const uint8_t* get() const { return _data.get(); }

            template <typename T>
            T getInt(const UTF8Str& location);
            template <typename T>
            T getIntArray(const UTF8Str& location);
            template <typename T>
            T getFloat(const UTF8Str& location);
            template <typename T>
            T getFloatArray(const UTF8Str& location);
            bool getBool(const UTF8Str& location);
            bool getBoolArray(const UTF8Str& location);
            UTF8Str getString(const UTF8Str& location);
            UTF8Str* getStringArray(const UTF8Str& location);

        private:
            // Variables
            int64_t _size;
            std::shared_ptr<const uint8_t> _data;
    };
}
