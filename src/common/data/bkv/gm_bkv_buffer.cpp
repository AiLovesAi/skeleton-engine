#include "gm_bkv_buffer.hpp"

#include "../gm_buffer_memory.hpp"

#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_Buffer::endCompound() {
        depth--;
        if (depth < 0) {
            std::stringstream msg;
            msg << "BKV escaped compound at index " << head << ".";
            throw std::out_of_range(msg.str());
        }

        try {
            BufferMemory::checkResize(bkv, head + 1, head, capacity);
        } catch (std::exception e) {
            throw e;
        }

        bkv[head] = BKV::BKV_END;
        head++;
        valHead = head;
        tagHead = head;
        tag = 0;
    }

    void BKV_Buffer::endKV(BKV_Buffer& buf, const char c) {
        buf.valHead = buf.head;
        if (buf.tag & BKV::BKV_ARRAY) {
            if (c == '}') {
                std::stringstream msg;
                msg << "Closing character is '}' when in BKV array at index: " << buf.head << ".";
                throw std::invalid_argument(msg.str());
            }
            buf.state = BKV_State::arrayState();
            try {
                buf.state->parse(buf, c);
            } catch (std::exception e) {
                throw e;
            }
        } else {
            buf.bkv[buf.tagHead] = buf.tag;
            if (c == '}') {
                try {
                    buf.endCompound();
                } catch (std::exception e) {
                    throw e;
                }
            }
            buf.state = BKV_State::keyState();
            buf.tagHead = buf.head;
            buf.tag = 0;
        }
    }
};
