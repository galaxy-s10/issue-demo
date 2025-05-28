#include <Windows.h>
#include <WtsApi32.h>
#include <UserEnv.h>
#include <stdio.h>
#include <strsafe.h>
#include <codecvt>
#include <fstream>
#include <locale>
#include <iostream>
#include <string>  // 包含 std::string 和 std::wstring
#include <vector>  // 包含 std::vector
#include <algorithm>

#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Userenv.lib")

#define SERVICE_NAME L"SystemDesktopService"
SERVICE_STATUS_HANDLE g_hServiceStatus;
SERVICE_STATUS g_ServiceStatus = {0};
BOOL g_bRunning = TRUE;
std::wstring g_lpExePath = L"C:\\BilldDesk\\hhhh.exe";
std::wstring g_lpExeArg = L"--billddeskwinstart";

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

// 日志记录函数
void LogEvent(const wchar_t* format, ...) {
    wchar_t buffer[1024];
    va_list args;
    va_start(args, format);
    vswprintf_s(buffer, format, args);
    va_end(args);
    OutputDebugString(buffer);

    // 写入事件日志（服务中推荐方式）
    HANDLE hEventSource = RegisterEventSourceW(NULL, SERVICE_NAME);
    if (hEventSource) {
        LPCWSTR strings[1] = {buffer};
        ReportEventW(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, strings, NULL);
        DeregisterEventSource(hEventSource);
    }
}

// 服务控制处理函数
DWORD WINAPI ServiceCtrlHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext) {
    switch (dwControl) {
    case SERVICE_CONTROL_STOP:
        g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        g_bRunning = FALSE;
        LogEvent(L"服务正在停止...\n");
        break;
    default:
        break;
    }
    SetServiceStatus(g_hServiceStatus, &g_ServiceStatus);
    return NO_ERROR;
}

// 创建系统权限桌面进程
BOOL CreateSystemDesktopProcess() {
    HANDLE hToken = NULL, hDupToken = NULL;
    DWORD dwSessionId = WTSGetActiveConsoleSessionId();
    LogEvent(L"活动会话ID: %d\n", dwSessionId);

    // 获取当前进程令牌
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken)) {
        LogEvent(L"OpenProcessToken失败 (错误: %d)\n", GetLastError());
        return FALSE;
    }

    // 复制令牌并设置会话
    if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hDupToken)) {
        LogEvent(L"DuplicateTokenEx失败 (错误: %d)\n", GetLastError());
        CloseHandle(hToken);
        return FALSE;
    }

    if (!SetTokenInformation(hDupToken, TokenSessionId, &dwSessionId, sizeof(DWORD))) {
        LogEvent(L"SetTokenInformation失败 (错误: %d)\n", GetLastError());
        CloseHandle(hDupToken);
        CloseHandle(hToken);
        return FALSE;
    }

    // 准备创建进程参数
    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi = {0};
    LPVOID pEnv = NULL;
    si.lpDesktop = const_cast<LPWSTR>(L"winsta0\\default");

    // 创建环境块
    if (!CreateEnvironmentBlock(&pEnv, hDupToken, FALSE)) {
        LogEvent(L"CreateEnvironmentBlock失败 (错误: %d)\n", GetLastError());
        CloseHandle(hDupToken);
        CloseHandle(hToken);
        return FALSE;
    }

    // 打印 g_lpExePath 的值（调试用）
    // std::wcout << L"即将启动的进程路径: " << g_lpExePath << std::endl;
    LogMessage(L"即将启动的进程路径: %ls\n", g_lpExePath.c_str());  // 如果需要记录到日志文件

    // g_lpExePath.erase(g_lpExePath.find_last_not_of(L' ') + 1);

    // LogMessage(L"去除空格: %ls\n", g_lpExePath.c_str());  // 如果需要记录到日志文件

    // std::wstring fullCmdLine = L"\"" + g_lpExePath + L"\" " + g_lpExeArg;

    g_lpExePath.erase(std::remove(g_lpExePath.begin(), g_lpExePath.end(), L'\0'), g_lpExePath.end());

    std::wstring fullCmdLine = L"\"" + g_lpExePath + L"\" " + g_lpExeArg;

    LogMessage(L"xx1: %ls\n", g_lpExePath.c_str());  // 如果需要记录到日志文件

    // std::wcout << L"fullCmdLine: " << fullCmdLine << std::endl;

    // std::wstring fullCmdLine = L"\"" + std::wstring(L"C:\\xdyun2033\\XD-Yun.exe") + L"\" " + g_lpExeArg;
    // std::wstring fullCmdLine = L"\"C:\\xdyun2033\\XD-Yun.exe\" --billddeskwinstart";
    // 最终的结果需要是："C:\xdyun2033\XD-Yun.exe" --billddeskwinstart
    LogMessage(L"fullCmdLinefullCmdLine: %ls\n", fullCmdLine.c_str());  // 如果需要记录到日志文件

    // 启动进程
    BOOL bSuccess = CreateProcessAsUserW(
        hToken,
        NULL,                                     // 不指定 lpApplicationName
        const_cast<LPWSTR>(fullCmdLine.c_str()),  // 必须传递可修改的字符串
        NULL,
        NULL,
        FALSE,
        CREATE_UNICODE_ENVIRONMENT,
        pEnv,
        NULL,
        &si,
        &pi);
    // BOOL bSuccess = CreateProcessAsUserW(
    //     hDupToken,
    //     g_lpExePath.c_str(),  // 使用 .c_str() 获取 const wchar_t*
    //     NULL,
    //     NULL,
    //     NULL,
    //     FALSE,
    //     CREATE_UNICODE_ENVIRONMENT,
    //     pEnv,
    //     NULL,
    //     &si,
    //     &pi);

    if (bSuccess) {
        LogMessage(L"进程创建成功！PID: %d\n", pi.dwProcessId);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        LogMessage(L"CreateProcessAsUser失败 (错误: %d)\n", GetLastError());
    }

    // 清理资源
    DestroyEnvironmentBlock(pEnv);
    CloseHandle(hDupToken);
    CloseHandle(hToken);
    return bSuccess;
}

// 服务主函数
VOID WINAPI ServiceMain(DWORD dwArgc, LPWSTR* lpszArgv) {
    g_hServiceStatus = RegisterServiceCtrlHandlerEx(SERVICE_NAME, ServiceCtrlHandler, NULL);
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    SetServiceStatus(g_hServiceStatus, &g_ServiceStatus);

    // 记录所有参数
    LogMessage(L"参数总数: %d\n", dwArgc);
    for (DWORD i = 0; i < dwArgc; i++) {
        LogMessage(L"参数[%d]: %s\n", i, lpszArgv[i]);
    }

    LogMessage(L"服务启动成功\n");
    while (g_bRunning) {
        if (!CreateSystemDesktopProcess()) {
            LogMessage(L"进程创建失败。\n");
            // Sleep(5000);
            break;
        } else {
            break;
        }
    }
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(g_hServiceStatus, &g_ServiceStatus);
}

// 主入口点
int main(int argc, char* argv[]) {
    // 打开日志文件（追加模式）
    std::ofstream logFile("C:\\service_args.log", std::ios::app);

    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file!" << std::endl;
        return 1;
    }

    // 将窄字符参数转换为宽字符
    std::vector<std::wstring> wideArgs;
    for (int i = 0; i < argc; ++i) {
        int wideSize = MultiByteToWideChar(CP_ACP, 0, argv[i], -1, NULL, 0);
        if (wideSize == 0) {
            std::cerr << "MultiByteToWideChar failed for argument " << i << std::endl;
            continue;
        }
        std::wstring wideArg(wideSize, L'\0');
        MultiByteToWideChar(CP_ACP, 0, argv[i], -1, &wideArg[0], wideSize);
        wideArgs.push_back(wideArg);
    }

    // 写入每个宽字符参数
    for (size_t i = 0; i < wideArgs.size(); ++i) {
        logFile << "Argumentx " << i << ": " << std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(wideArgs[i])
                << std::endl;
    }

    // 检查是否有足够的参数
    if (argc < 2) {
        LogMessage(L"错误：未提供足够的参数。\n");
        std::cerr << "错误：未提供足够的参数。\n";
        return 1;
    }
    // 解析 --billddir 参数
    g_lpExePath = wideArgs[2];

    SERVICE_TABLE_ENTRYW ServiceTable[] = {{(LPWSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTIONW)ServiceMain}, {NULL, NULL}};
    if (!StartServiceCtrlDispatcherW(ServiceTable)) {
        LogEvent(L"服务派发失败 (错误: %d)\n", GetLastError());
    }
    return 0;
}