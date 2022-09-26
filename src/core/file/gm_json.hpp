#pragma once

#include <string>

namespace game {
    class JSON {
        public:
            static std::string load(const std::string& file);
            static void save(const std::string& file, const std::string& data);
            
            static int readInt(const std::string& data, const std::string& location);
            static float readFloat(const std::string& data, const std::string& location);
            static double readDouble(const std::string& data, const std::string& location);
            static bool readBool(const std::string& data, const std::string& location);
            static std::string readString(const std::string& data, const std::string& location);

            static void writeInt(const std::string& data, const std::string& location, const int x);
            static void writeFloat(const std::string& data, const std::string& location, const float x);
            static void writeDouble(const std::string& data, const std::string& location, const double x);
            static void writeBool(const std::string& data, const std::string& location, const bool x);
            static void writeString(const std::string& data, const std::string& location, const std::string& x);
    };
}
