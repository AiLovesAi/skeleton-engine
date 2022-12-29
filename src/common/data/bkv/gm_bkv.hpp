#pragma once

#include "states/gm_bkv_state.hpp"

#include <cstdint>
#include <memory>

namespace game {
    class BKV {
        public:
            // Types
            template <typename T>
            struct BKVTypeMap { static const uint8_t tagID; };
            
            template <typename T>
            struct BKVSuffixMap { static const char suffix[]; };
            
            typedef struct UTF8Str_ {
                int64_t len;
                std::shared_ptr<char> str;
            } UTF8Str;
            
            typedef struct BKV_t_ {
                int64_t size;
                std::shared_ptr<uint8_t> data;
            } BKV_t;

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
                BKV_END, // End of compound (does not have a name)
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
                BKV_I8_ARRAY = BKV_I8 | BKV_ARRAY, // BKV_UI32 (size) + Array of BKV_I8
                BKV_UI8 = BKV_I8 | BKV_UNSIGNED, // 1B Unsigned Int
                BKV_UI8_ARRAY = BKV_UI8 | BKV_ARRAY, // BKV_UI32 (size) + Array of BKV_UI8

                BKV_I16_ARRAY = BKV_I16 | BKV_ARRAY, // BKV_UI32 (size) + Array of BKV_I16
                BKV_UI16 = BKV_I16 | BKV_UNSIGNED, // 2B Unsigned Int
                BKV_UI16_ARRAY = BKV_UI16 | BKV_ARRAY, // BKV_UI32 (size) + Array of BKV_UI16

                BKV_I32_ARRAY = BKV_I32 | BKV_ARRAY, // BKV_UI32 (size) + Array of BKV_I32
                BKV_UI32 = BKV_I32 | BKV_UNSIGNED, // 4B Unsigned Int
                BKV_UI32_ARRAY = BKV_UI32 | BKV_ARRAY, // BKV_UI32 (size) + Array of BKV_UI32

                BKV_I64_ARRAY = BKV_I64 | BKV_ARRAY, // BKV_UI32 (size) + Array of BKV_I64
                BKV_UI64 = BKV_I64 | BKV_UNSIGNED, // 8B Unsigned Int
                BKV_UI64_ARRAY = BKV_UI64 | BKV_ARRAY, // BKV_UI32 (size) + Array of BKV_UI64

                BKV_FLOAT_ARRAY = BKV_FLOAT | BKV_ARRAY, // BKV_UI32 (size) + Array of BKV_FLOAT
                BKV_DOUBLE_ARRAY = BKV_DOUBLE | BKV_ARRAY, // BKV_UI32 (size) + Array of BKV_DOUBLE
                BKV_STR_ARRAY = BKV_STR | BKV_ARRAY, // BKV_UI32 (size) + Array of BKV_STR
            };

            // Constructors
            BKV() {}
            BKV(const BKV_t& bkv);
            BKV(const UTF8Str& stringified);
            ~BKV() { std::free(buffer_); };

            // Functions
            template<typename T>
            void set(const std::string& name, const T data);
            template<typename T>
            void setList(const std::string& name, const T* data, const uint32_t size);
            void setStr(const std::string& name, const UTF8Str& data);
            void setStrList(const std::string& name, const UTF8Str* data, const uint16_t size);
            
            template<typename T>
            T get(const std::string& name);
            const uint8_t* get() { return buffer_; }
            int64_t size() { return head_; }

        private:
            // Functions
            void resizeBuffer(const int64_t size);
            void write(const BKV_t& bkv);

            static UTF8Str sbkvFromBKV(const BKV_t& bkv);

            static BKV_t bkvFromSBKV(const UTF8Str& stringified);
            static BKV_t bkvCompound(const UTF8Str& name);
            static inline uint8_t bkvCompoundEnd() { return BKV::BKV_END; }
            template<typename T>
            static BKV_t bkv(const UTF8Str& name, const T data);
            template<typename T>
            static BKV_t bkvList(const UTF8Str& name, const T* data, const uint32_t size);
            static BKV_t bkvStr(const UTF8Str& name, const UTF8Str& data);
            static BKV_t bkvStrList(const UTF8Str& name, const UTF8Str* data, const uint32_t size);

            // Variables
            uint8_t* buffer_ = nullptr;
            int64_t head_ = 0;
            int64_t capacity_ = 0;
    };
}
