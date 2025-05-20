#include <napi.h>
#include <Windows.h>
#include <WtsApi32.h>
#include <string>
#include <cstdlib>

#pragma comment(lib, "WtsApi32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "User32.lib")

// 辅助进程路径（临时生成）
std::wstring GetHelperPath() {
    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    return std::wstring(tempPath) + L"mouse_helper.exe";
}

// 移动鼠标的JS接口
Napi::Value MoveCursor(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 2 || !info[0].IsNumber() || !info[1].IsNumber()) {
        throw Napi::Error::New(env, "Expected x, y coordinates as numbers");
    }

    int x = info[0].As<Napi::Number>().Int32Value();
    int y = info[1].As<Napi::Number>().Int32Value();

    // 1. 获取用户会话Token
    DWORD sessionId = WTSGetActiveConsoleSessionId();
    HANDLE hToken = NULL;
    if (!WTSQueryUserToken(sessionId, &hToken)) {
        throw Napi::Error::New(env, "WTSQueryUserToken failed");
    }

    // 2. 启动辅助进程
    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi;
    std::wstring cmdLine = L"\"" + GetHelperPath() + L"\" " + std::to_wstring(x) + L" " + std::to_wstring(y);

    if (!CreateProcessAsUserW(
            hToken,
            NULL,
            const_cast<LPWSTR>(cmdLine.c_str()),
            NULL,
            NULL,
            FALSE,
            CREATE_NO_WINDOW,
            NULL,
            NULL,
            &si,
            &pi)) {
        CloseHandle(hToken);
        throw Napi::Error::New(env, "CreateProcessAsUser failed");
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    CloseHandle(hToken);

    return env.Undefined();
}

// 初始化Addon
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("moveCursor", Napi::Function::New(env, MoveCursor));
    return exports;
}

NODE_API_MODULE(mouse_addon, Init)