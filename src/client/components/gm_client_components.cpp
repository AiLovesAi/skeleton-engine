#include "gm_client_components.hpp"

namespace game {
    void ClientComponents::render(const float128_t lag) {
        render_.render(lag);
    }
}
