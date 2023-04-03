#include "gm_bkv_parser.hpp"

#include "gm_bkv.hpp"
#include "../../headers/string.hpp"
#include "../gm_endianness.hpp"
#include "states/gm_bkv_parser_state_complete.hpp"

#include <stdexcept>

namespace game {
    void BKV_Parser::openCompound() {
        if (_tag & BKV::BKV_ARRAY) {
            UTF8Str msg = FormatString::formatString("Compound in unclosed BKV array at index %d", _charactersRead);
            throw std::runtime_error(msg.get());
        }
        
        // Increase compound depth and return to name state for next input
        try {
            StringBuffer::checkResize(_buffer.bkv_, _buffer._head + BKV::BKV_COMPOUND_SIZE + 1, _buffer._head, _buffer._capacity);
        } catch (std::runtime_error &e) { throw; }
        _buffer.bkv_[_buffer._tagHead] = BKV::BKV_COMPOUND;
        if (_buffer._tagHead == _buffer._head) { // This will be true for the opening compound
            _buffer._head++;
            _buffer.bkv_[_buffer._head] = '\0'; // 0 for name length
            _buffer._head++;
        }
        _depth.push(_buffer._head);
        _buffer._head += BKV::BKV_COMPOUND_SIZE;
        _buffer._valHead = _buffer._head;
        _buffer._tagHead = _buffer._head;
        
        UTF8Str m = FormatString::formatString("Opening compound with new depth: %d", _depth.size());
        Logger::log(LOG_INFO, m);
        if (_depth.size() > BKV::BKV_COMPOUND_DEPTH_MAX) {
            std::stringstream msg;
            msg << "Reached maximum compound depth in SBKV at index " << _charactersRead << ": " << _depth.size() << "/" << BKV::BKV_COMPOUND_DEPTH_MAX;
            throw std::runtime_error(msg.str());
        }
    }

    void BKV_Parser::closeCompound() {
        try {
            StringBuffer::checkResize(_buffer.bkv_, _buffer._head + 1, _buffer._head, _buffer._capacity);
        } catch (std::runtime_error &e) { throw; }
        _buffer.bkv_[_buffer._head] = BKV::BKV_END;
        _buffer._head++;

        int64_t size = _buffer._head - _depth.top() + sizeof(uint32_t);
        if ((size <= 0) || (size > BKV::BKV_COMPOUND_MAX)) {
            std::stringstream msg;
            msg << "BKV compound bigger than maximum size at index" << _depth.top() << ": " << size << "/" << BKV::BKV_COMPOUND_MAX;
            throw std::runtime_error(msg.str());
        }
        uint32_t len = Endianness::hton(static_cast<uint32_t>(size));
        std::memcpy(_buffer.bkv_ + _depth.top(), &len, BKV::BKV_COMPOUND_SIZE);
        std::stringstream m;
        m << "Closing compound with new depth: " << _depth.size() - 1 << " Length of " << size << " (net " << len << ") put at " << _depth.top();
        Logger::log(LOG_INFO, m.str());
        _depth.pop();
        _buffer._valHead = _buffer._head;
        _buffer._tagHead = _buffer._head;
        _tag = 0;
        
        if (!_depth.size()) {
            // Depth has returned to zero, meaning the enclosing compound is closed; BKV is now finished.
            _stateTree.push(&_completeState);
            
            // Reallocate to use only as much memory as necessary
            _buffer._capacity = _buffer._head;
            _buffer.bkv_ = static_cast<uint8_t*>(std::realloc(_buffer.bkv_, _buffer._capacity));
        }
    }

    void BKV_Parser::endKV(const char c) {
        std::stringstream m;
        m << "Ending key value with character: " << c;
        Logger::log(LOG_INFO, m.str());

        _buffer._valHead = _buffer._head;
        if (_tag & BKV::BKV_ARRAY) {
            if (c == '}') {
                std::stringstream msg;
                msg << "Closing character is '}' when in SBKV array at index: " << _charactersRead;
                throw std::runtime_error(msg.str());
            }

            // Make sure tag has not changed
            if (!_arrayState.arrayTag_) {
                _arrayState.arrayTag_ = _tag;
            } else if (_tag != _arrayState.arrayTag_) {
                std::stringstream msg;
                msg << "Array value changed data type at index: " << _charactersRead;
                reset();
                throw std::runtime_error(msg.str());
            }

            _stateTree.push(&_arrayState);
            try { state()->parse(*this, c); } catch (std::runtime_error &e) { throw; }
        } else {
            if (c == ']') {
                std::stringstream msg;
                msg << "Closing character is ']' when not in SBKV array at index: " << _charactersRead;
                throw std::runtime_error(msg.str());
            }
            
            _buffer.bkv_[_buffer._tagHead] = _tag;
            if (c == '}') {
                try { closeCompound(); } catch (std::runtime_error &e) { throw; }
            }
            if (_depth.size()) _stateTree.pop();
            _buffer._tagHead = _buffer._head;
            _tag = 0;
        }
    }
};
