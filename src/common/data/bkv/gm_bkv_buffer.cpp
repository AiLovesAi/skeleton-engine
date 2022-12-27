#include "gm_bkv_buffer.hpp"

#include "../gm_buffer_memory.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_Buffer::endCompound() {
        depth--;

        try {
            BufferMemory::checkResize(bkv, head + 1, head, capacity);
        } catch (std::exception e) { throw e; }
        bkv[head] = BKV::BKV_END;
        head++;
        valHead = head;
        tagHead = head;
        tag = 0;
        
        if (depth < 0) {
            std::stringstream msg;
            msg << "BKV escaped compound at index " << head << ".";
            throw std::out_of_range(msg.str());
        } else if (!depth) {
            // Depth has returned to zero, meaning the enclosing compound is closed; BKV is now finished.
            state = BKV_State::completeState();
        }
    }

    void BKV_Buffer::endKV(const char c) {
        valHead = head;
        if (tag & BKV::BKV_ARRAY) {
            if (c == '}') {
                std::stringstream msg;
                msg << "Closing character is '}' when in BKV array at index: " << head << ".";
                throw std::invalid_argument(msg.str());
            }
            state = BKV_State::arrayState();
            try { state->parse(*this, c); } catch (std::exception e) { throw e; }
        } else {
            bkv[tagHead] = tag;
            if (c == '}') {
                try { endCompound(); } catch (std::exception e) { throw e; }
            }
            state = BKV_State::keyState();
            tagHead = head;
            tag = 0;
        }
    }
};
