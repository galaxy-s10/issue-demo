#include <Windows.h>
#include <shellapi.h>

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR cmdLine, int) {
    int argc;
    LPWSTR* argv = CommandLineToArgvW(cmdLine, &argc);
    if (argc >= 3) {
        int x = _wtoi(argv[1]);
        int y = _wtoi(argv[2]);
        SetCursorPos(x, y);
    }
    LocalFree(argv);
    return 0;
}