#include <napi.h>
#include <thread>
#include <chrono>
#include <iostream>

#ifdef _WIN32
#include <windows.h>  // Windows API for mouse control
#endif

namespace __node_addon_api_mouse__ {

// 鼠标移动方法的实现
Napi::Object MoveMouse(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // 创建一个返回的对象
    Napi::Object result = Napi::Object::New(env);

    // 检查参数数量
    if (info.Length() < 4) {
        result.Set(Napi::String::New(env, "status"), Napi::String::New(env, "error"));
        result.Set(Napi::String::New(env, "message"), Napi::String::New(env, "Expected two arguments"));
        return result;
    }

    // 检查参数类型
    if (!info[0].IsNumber() || !info[1].IsNumber() || !info[2].IsNumber() || !info[3].IsNumber()) {
        result.Set(Napi::String::New(env, "status"), Napi::String::New(env, "error"));
        result.Set(Napi::String::New(env, "message"), Napi::String::New(env, "Expected three numbers"));
        return result;
    }

    // 获取参数
    int x = info[0].As<Napi::Number>().Int32Value();
    int y = info[1].As<Napi::Number>().Int32Value();
    int count = info[2].As<Napi::Number>().Int32Value();     // 点击次数
    int interval = info[3].As<Napi::Number>().Int32Value();  // 每次点击的间隔 (毫秒)

#ifdef _WIN32

    for (int i = 0; i < count; ++i) {
        // 移动鼠标
        // SetCursorPos(x, y);

        // 模拟鼠标点击
        // mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);  // 按下左键
        // mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);    // 释放左键

        POINT p;
        if (GetCursorPos(&p)) {
            std::cout << "Mouse Position - X: " << p.x << ", Y: " << p.y << std::endl;

            result.Set(Napi::String::New(env, "status"), Napi::String::New(env, "success"));
            result.Set(Napi::String::New(env, "x"), Napi::Number::New(env, p.x));
            result.Set(Napi::String::New(env, "y"), Napi::Number::New(env, p.y));
        } else {
            DWORD error = GetLastError();  // 获取错误代码
            std::cout << "Failed to get mouse position. Error Code: " << error << std::endl;

            std::cout << "GetCursorPos cuole!" << std::endl;  // 打印到控制台

            result.Set(Napi::String::New(env, "status"), Napi::String::New(env, "error"));
            result.Set(Napi::String::New(env, "message"), Napi::String::New(env, "Failed to get mouse position"));
        }

        // 等待指定间隔
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    }

    result.Set(Napi::String::New(env, "status"), Napi::String::New(env, "success"));
    result.Set(Napi::String::New(env, "message"), Napi::String::New(env, "Mouse moved"));
    result.Set(Napi::String::New(env, "x"), Napi::Number::New(env, x));
    result.Set(Napi::String::New(env, "y"), Napi::Number::New(env, y));
    result.Set(Napi::String::New(env, "count"), Napi::Number::New(env, y));
    result.Set(Napi::String::New(env, "interval"), Napi::Number::New(env, y));
#else
    result.Set(Napi::String::New(env, "status"), Napi::String::New(env, "error"));
    result.Set(Napi::String::New(env, "message"), Napi::String::New(env, "Not supported on this platform"));
#endif

    return result;

    //   return Napi::String::New(env, "hellox-world"); // 返回hello-world
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    // 在exports对象挂上hello属性，其值为Hello函数，也可以用以下的Nan::Export宏实现
    exports.Set(Napi::String::New(env, "moveMouse"), Napi::Function::New(env, MoveMouse));

    return exports;
}

NODE_API_MODULE(addon, Init)

}  // namespace __node_addon_api_mouse__