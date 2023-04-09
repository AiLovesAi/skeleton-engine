#include "gm_bkv_parser.hpp"

#include "gm_bkv.hpp"
#include "../../headers/string.hpp"
#include "../gm_endianness.hpp"
#include "states/gm_bkv_parser_state_complete.hpp"

#include <stdexcept>

namespace game {
    void BKV_Parser::openCompound() {
        // Input validation
        if (_tag & BKV::BKV_ARRAY) {
            UTF8Str msg = FormatString::formatString("Compound in unclosed BKV array at %ld", _charactersRead);
            throw std::runtime_error(msg.get());
        }
        
        // Increase compound depth and return to name state for next input
        try {
            StringBuffer::checkResize(_buffer._bkv, _buffer._head + BKV::BKV_COMPOUND_SIZE + 1, _buffer._head, _buffer._capacity);
        } catch (std::runtime_error &e) { throw; }
        _buffer._bkv[_buffer._tagHead] = BKV::BKV_COMPOUND;
        if (_buffer._tagHead == _buffer._head) { // This will be true for the opening compound
            _buffer._head++;
            _buffer._bkv[_buffer._head++] = 0; // 0 for name length
        }
        _depth.push(_buffer._head);
        _buffer._head += BKV::BKV_COMPOUND_SIZE;
        _buffer._valHead = _buffer._head;
        _buffer._tagHead = _buffer._head;
        
        if (_depth.size() > BKV::BKV_COMPOUND_DEPTH_MAX) {
            UTF8Str msg = FormatString::formatString("Reached maximum compound depth in SBKV at index %ld: %lu/%u",
                _charactersRead, _depth.size(), BKV::BKV_COMPOUND_DEPTH_MAX
            );
            throw std::runtime_error(msg.get());
        }
    }

    void BKV_Parser::closeCompound() {
        try {
            StringBuffer::checkResize(_buffer._bkv, _buffer._head + 1, _buffer._head, _buffer._capacity);
        } catch (std::runtime_error &e) { throw; }
        _buffer._bkv[_buffer._head++] = BKV::BKV_END;

        int64_t size = _buffer._head - _depth.top() + sizeof(uint32_t);
        if ((size <= 0) || (size > BKV::BKV_COMPOUND_MAX)) {
            UTF8Str msg = FormatString::formatString("BKV compound bigger than maximum size at index %d: %ld/%u",
                _depth.top(), size, BKV::BKV_COMPOUND_MAX
            );
            throw std::runtime_error(msg.get());
        }
        const uint32_t len = Endianness::hton(static_cast<uint32_t>(size));
        std::memcpy(_buffer._bkv + _depth.top(), &len, BKV::BKV_COMPOUND_SIZE);
        _depth.pop();
        _buffer._valHead = _buffer._head;
        _buffer._tagHead = _buffer._head;
        _tag = 0;
        
        if (!_depth.size()) {
            // Depth has returned to zero, meaning the enclosing compound is closed; BKV is now finished.
            _stateTree.push(&_completeState);
            
            // Reallocate to use only as much memory as necessary
            _buffer._capacity = _buffer._head;
            _buffer._bkv = static_cast<uint8_t*>(std::realloc(_buffer._bkv, _buffer._capacity));
        }
    }

    void BKV_Parser::endKV(const char c) {
        _buffer._valHead = _buffer._head;
        if (_tag & BKV::BKV_ARRAY) {
            // Input validation
            if (c == '}') {
                UTF8Str msg = FormatString::formatString("Closing character is '}' when in SBKV array at index: %ld", _charactersRead);
                throw std::runtime_error(msg.get());
            }

            // Make sure tag has not changed
            if (!_arrayState._arrayTag) {
                _arrayState._arrayTag = _tag;
            } else if (_tag != _arrayState._arrayTag) {
                UTF8Str msg = FormatString::formatString("Array value changed data type at index: %ld", _charactersRead);
                reset();
                throw std::runtime_error(msg.get());
            }

            _stateTree.push(&_arrayState);
            try { state()->parse(*this, c); } catch (std::runtime_error &e) { throw; }
        } else {
            // Input validation
            if (c == ']') {
                UTF8Str msg = FormatString::formatString("Closing character is ']' when not in SBKV array at index: %ld",
                    _charactersRead
                );
                throw std::runtime_error(msg.get());
            }
            
            _buffer._bkv[_buffer._tagHead] = _tag;
            if (c == '}') {
                try { closeCompound(); } catch (std::runtime_error &e) { throw; }
            }
            if (_depth.size()) _stateTree.pop();
            _buffer._tagHead = _buffer._head;
            _tag = 0;
        }
    }
};
