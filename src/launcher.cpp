extern "C" {
    #include <stdio.h>
    #include <stdint.h>
    #include <stdlib.h>
    #include <string.h>

    #ifdef _WIN32
    #include <windows.h>
    #endif

    int cmdOptionExists(char** argv, const int argc, const char* option) {
        for (int i = 1; i < argc && argv[i][0] == '-'; i++) {
            if (!strcmp(argv[i], option)) return 1;
        }
        return 0;
    }

    int main (int argc, char** argv)
    {
        // TODO Use absolute directory and ensure files exist
        // Check if server
        if (cmdOptionExists(argv, argc, "--server")) {
            // Launch Server
            printf("Launching server.\n");
    #ifdef _WIN32
            WinExec("./bin/host.exe", SW_SHOW);
    #else
            system("./bin/host");
    #endif
        } else {
            // Launch client
            printf("Launching client.\n");
    #ifdef _WIN32
            WinExec("./bin/game.exe", SW_HIDE);
    #else
            system("./bin/game");
    #endif
        }

        puts("Exited.\n");
        
        return EXIT_SUCCESS;
    }

}
