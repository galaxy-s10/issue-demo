#include <Windows.h>
#include <WtsApi32.h>
#include <UserEnv.h>
#include <stdio.h>
#include <strsafe.h>
#include <codecvt>
#include <fstream>
#include <locale>

#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Userenv.lib")

#define SERVICE_NAME L"SystemDesktopService"
SERVICE_STATUS_HANDLE g_hServiceStatus;
SERVICE_STATUS g_ServiceStatus = {0};
BOOL g_bRunning = TRUE;

void LogMessage(const wchar_t* format, ...) {
    // 获取格式化的宽字符日志
    wchar_t wbuffer[1024];
    va_list args;
    va_start(args, format);
    _vsnwprintf_s(wbuffer, _countof(wbuffer), _TRUNCATE, format, args);
    va_end(args);

    // 转换为UTF-8
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string utf8Log = converter.to_bytes(wbuffer);

    // 写入文件（带BOM）
    std::ofstream logFile("C:\\service_args.log", std::ios::out | std::ios::app | std::ios::binary);

    // 写入UTF-8 BOM（可选）
    static bool bomWritten = false;
    if (!bomWritten) {
        const char bom[] = "\xEF\xBB\xBF";
        logFile.write(bom, sizeof(bom) - 1);
        bomWritten = true;
    }

    logFile << utf8Log;
    logFile.close();
}

// 主入口点
int main(DWORD dwArgc, LPWSTR* lpszArgv[]) {
    // 记录所有参数
    LogMessage(L"xx参数总数: %d\n", dwArgc);
    for (DWORD i = 0; i < dwArgc; i++) {
        LogMessage(L"xx参数[%d]: %s\n", i, lpszArgv[i]);
    }

    return 0;
}