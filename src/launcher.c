#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int cmdOptionExists(char** argv, const int argc, const char* option) {
    for (size_t i = 1; i < argc && argv[i][0] == '-'; i++) {
        if (!strcmp(argv[i], option)) return 1;
    }
    return 0;
}

int main (int argc, char** argv)
{
    // Check if server
    if (cmdOptionExists(argv, argv + argc, "--server")) {
        // Launch Server
    } else {
        // Launch client
    }
    
    return EXIT_SUCCESS;
}
