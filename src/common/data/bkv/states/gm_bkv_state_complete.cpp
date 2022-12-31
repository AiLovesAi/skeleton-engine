#include "gm_bkv_state_complete.hpp"

#include "../gm_bkv_buffer.hpp"

#include <cctype>
#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_State_Complete::parse(BKV_Buffer& buf, const char c) {
        buf.charactersRead_++;
        
        if (std::isspace(c) || (c == '\0')) {
            return;
        } else {
            std::stringstream msg;
            msg << "Enclosing BKV compound has already been closed and the bkv is finished at index: " << buf.charactersRead_ << ".";
            throw std::runtime_error(msg.str());
        }
    }
}
