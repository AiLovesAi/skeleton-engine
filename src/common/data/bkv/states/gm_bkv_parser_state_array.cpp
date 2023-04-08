#include "gm_bkv_parser_state_array.hpp"

#include "../gm_bkv.hpp"
#include "../gm_bkv_parser.hpp"
#include "../../../headers/string.hpp"
#include "../../gm_endianness.hpp"

#include <stdexcept>

namespace game {
    void BKV_Parser_State_Array::parse(BKV_Parser& parser, const char c) {
        if (!_arrayStart) {
            _arrayStart = parser._buffer._head;
            _arrayTagHead = parser._buffer._tagHead;
            try {
                StringBuffer::checkResize(parser._buffer._bkv, parser._buffer._head + BKV::BKV_ARRAY_SIZE, parser._buffer._head, parser._buffer._capacity);
            } catch (std::runtime_error &e) {
                reset();
                throw;
            }
            parser._buffer._head += BKV::BKV_ARRAY_SIZE;
            
            _size++;
            parser._stateTree.push(&parser._findTagState);
            try {
                parser.state()->parse(parser, c);
            } catch (std::runtime_error &e) {
                reset();
                throw;
            }
        } else {
            if (c == ',') {
                parser._tag &= BKV::BKV_FLAGS_ALL; // Clear the tag so it can be found again
                
                // Continue array
                _size++;
                if (_size > BKV::BKV_ARRAY_MAX) {
                    UTF8Str msg = FormatString::formatString("Too many indicies in SBKV array at %ld: %ld/%ld indicies.",
                        parser._charactersRead, _size, BKV::BKV_ARRAY_MAX
                    );
                    reset();
                }
                parser._stateTree.pop(); // Back to specific tag state
            } else if (c == ']') {
                // End array
                parser._buffer._bkv[_arrayTagHead] = parser._tag;
                uint32_t val = Endianness::hton(static_cast<uint16_t>(_size));
                std::memcpy(parser._buffer._bkv + _arrayStart, &val, BKV::BKV_ARRAY_SIZE);
                
                reset();
                parser._buffer._valHead = parser._buffer._head;
                parser._stateTree.pop(); // Back to specific tag state
                parser._stateTree.pop(); // Back to array state
                parser._stateTree.pop(); // Back to key state
                parser._buffer._tagHead = parser._buffer._head;
                parser._tag = 0;
            } else {
                UTF8Str msg = FormatString::formatString("Invalid character in SBKV array at index %ld: %02x",
                    parser._charactersRead, c
                );
                reset();
                throw std::runtime_error(msg.get());
            }
        }
    }
}
