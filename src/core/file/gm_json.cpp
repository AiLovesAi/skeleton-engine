#include "gm_json.hpp"

#include "gm_file.hpp"

#include <sstream>

namespace game {
    std::string JSON::load(const std::string& file) {
        std::stringstream data;

        // TODO Streamed read from file (lock its mutex)

        return data.str();
    }

    void JSON::save(const std::string& file, const std::string& data) {
        // TODO Streamed/buffered write to file (lock its mutex)
    }

    int JSON::readInt(const std::string& data, const std::string& location) {
        int x = 0;
        return x;
    }
}