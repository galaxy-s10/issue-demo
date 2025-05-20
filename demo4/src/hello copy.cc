#include <napi.h>
#include <windows.h>

namespace __node_addon_api_mouse__ {

// 鼠标移动方法的实现
Napi::String MoveMouse(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    SetCursorPos(100, 100);

    return Napi::String::New(env, "SetCursorPos-ok");
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "moveMouse"), Napi::Function::New(env, MoveMouse));

    return exports;
}

NODE_API_MODULE(addon, Init)

}  // namespace __node_addon_api_mouse__