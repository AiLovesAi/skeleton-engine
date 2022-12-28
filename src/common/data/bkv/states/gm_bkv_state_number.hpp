#pragma once

#include "gm_bkv_state.hpp"

namespace game {
    class BKV;
    class BKV_Buffer;

    class BKV_State_Number : public BKV_State { 
        public:
            // Functions
            void reset() {
                bufLen_ = 0;
                hasDecimal_ = false;
            }
            virtual void parse(BKV_Buffer& buf, const char c);

        private:
            // Functions
            template <typename T>
            void appendValue(BKV_Buffer& buf, const T value);
            template <typename T, typename TU>
            void parseInt(BKV_Buffer& buf, const int64_t min, const int64_t max, const uint64_t umax);
            void parseLong(BKV_Buffer& buf);
            void parseFloat(BKV_Buffer& buf);
            void parseDouble(BKV_Buffer& buf);

            // Variables
            char numBuf_[UINT8_MAX];
            size_t bufLen_ = 0;
            bool hasDecimal_ = false;
    };
}
