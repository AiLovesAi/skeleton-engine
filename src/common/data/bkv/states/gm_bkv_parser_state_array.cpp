#include "gm_bkv_parser_state_array.hpp"

#include "../gm_bkv.hpp"
#include "../gm_bkv_parser.hpp"
#include "../../string/gm_string.hpp"
#include "../../gm_endianness.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_Parser_State_Array::parse(BKV_Parser& parser, const char c) {
        std::stringstream m;
        m << "Array parsing character: '" << c << "', Array size: " << size_;
        Logger::log(LOG_INFO, m.str());

        if (!arrayStart_) {
            arrayStart_ = parser.buffer_.head_;
            arrayTagHead_ = parser.buffer_.tagHead_;
            try {
                String::checkResize(parser.buffer_.bkv_, parser.buffer_.head_ + BKV::BKV_ARRAY_SIZE, parser.buffer_.head_, parser.buffer_.capacity_);
            } catch (std::runtime_error &e) {
                reset();
                throw;
            }
            parser.buffer_.head_ += BKV::BKV_ARRAY_SIZE;
            
            size_++;
            parser.stateTree_.push(&parser.findTagState_);
            try {
                parser.state()->parse(parser, c);
            } catch (std::runtime_error &e) {
                reset();
                throw;
            }
        } else {
            if (c == ',') {
                parser.tag_ &= BKV::BKV_FLAGS_ALL; // Clear the tag so it can be found again
                
                // Continue array
                size_++;
                if (size_ > BKV::BKV_ARRAY_MAX) {
                    std::stringstream msg;
                    msg << "Too many indicies in SBKV array at index " << parser.charactersRead_ << ": " << size_ << "/" << BKV::BKV_ARRAY_MAX << " indicies.";
                    reset();
                }
                parser.stateTree_.pop(); // Back to specific tag state
            } else if (c == ']') {
                // End array
                parser.buffer_.bkv_[arrayTagHead_] = parser.tag_;
                uint32_t val = Endianness::hton(static_cast<uint16_t>(size_));
                std::memcpy(parser.buffer_.bkv_ + arrayStart_, &val, BKV::BKV_ARRAY_SIZE);
                
                reset();
                parser.buffer_.valHead_ = parser.buffer_.head_;
                parser.stateTree_.pop(); // Back to specific tag state
                parser.stateTree_.pop(); // Back to array state
                parser.stateTree_.pop(); // Back to key state
                parser.buffer_.tagHead_ = parser.buffer_.head_;
                parser.tag_ = 0;
            } else {
                std::stringstream msg;
                msg << "Invalid character in SBKV array at index " << parser.charactersRead_ << ": 0x" << std::hex << ((c & 0xf0) >> 4) << std::hex << (c & 0xf);
                reset();
                throw std::runtime_error(msg.str());
            }
        }
    }
}
