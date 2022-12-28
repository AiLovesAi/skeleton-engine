#include "gm_bkv_state_complete.hpp"

#include <cctype>
#include <sstream>
#include <stdexcept>

namespace game {
    void BKV_State_Complete::parse(BKV_Buffer& buf, const char c) {
        if (!std::isspace(c)) {
            std::stringstream msg;
            msg << "Enclosing BKV compound has already been closed and the bkv is finished at index: " << buf.head_ << ".";
            throw std::invalid_argument(msg.str());
        }
    }
}
