#include <algorithm>
#include <cstdlib>
#include <string>

char* cmdOption(char** begin, char** end, const std::string& option) {
    char** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option) {
    return std::find(begin, end, option) != end;
}

int main (int argc, char** argv)
{
    // TODO Parse args and launch server or client
    // Check if server
    if (cmdOptionExists(argv, argv + argc, "--server")) {
        // Launch Server
    } else {
        // Launch client
    }
    
    return EXIT_SUCCESS;
}
