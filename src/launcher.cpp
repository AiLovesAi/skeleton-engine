extern "C" {
    #include <stdio.h>
    #include <stdint.h>
    #include <stdlib.h>
    #include <string.h>

    #ifdef _WIN32
    #include <windows.h>
    #else
    #include <unistd.h>
    #include <limits.h>
    #endif
    
    inline int lastIndexOfString(const char* str, const char c) {
        int i;
        for (i = strlen(str) - 1; i > 0; i--) {
            if (str[i] == c) return i;
        }
        return i;
    }

    int cmdOptionExists(char** argv, const int argc, const char* option) {
        for (int i = 1; i < argc && argv[i][0] == '-'; i++) {
            if (!strcmp(argv[i], option)) return 1;
        }
        return 0;
    }

    int main (int argc, char** argv)
    {
    #ifdef _WIN32
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, sizeof(buffer));
        int index = lastIndexOfString(buffer, '\\');
        buffer[index + 1] = '\0';
    #else
        char buffer[PATH_MAX];
        if (argv[0] == '/') {
            strncpy(buffer, sizeof(buffer), argv[0]);
        } else {
            getcwd(buffer, sizeof(buffer));
            int len = strlen(buffer);
            if (buffer[len - 1] != '/') {
                buffer[len] = '/';
            }
            strcat(buffer, argv[0]);
        }
        
        int index = lastIndexOfString(buffer, '/');
        buffer[index + 1] = '\0';
    #endif
        // Check if server
        if (cmdOptionExists(argv, argc, "--server")) {
            // Launch Server
    #ifdef _WIN32
            strcat(buffer, "bin/host.exe");
            WinExec(buffer, SW_SHOW);
    #else
            strcat(buffer, "bin/host");
            system(buffer);
    #endif
        } else {
            // Launch client
    #ifdef _WIN32
            strcat(buffer, "bin/game.exe");
            WinExec(buffer, SW_HIDE);
    #else
            strcat(buffer, "bin/game");
            system(buffer);
    #endif
        }
        
        return EXIT_SUCCESS;
    }

}
