#pragma once
#include <windows.h>
#include <string> 
#include <WebView2.h>

using CreateEnvironmentFn = HRESULT (STDAPICALLTYPE *)(
    PCWSTR browserExecutableFolder,
    PCWSTR userDataFolder,
    ICoreWebView2EnvironmentOptions* environmentOptions,
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* environmentCreatedHandler
);

class WebViewManager{
private:

HMODULE lib_handler = nullptr;
CreateEnvironmentFn func_address = nullptr;  
public:
WebViewManager() = default;

~WebViewManager();
//deleting the copy oj and copy value of another oj
WebViewManager(const WebViewManager&) = delete;

WebViewManager& operator=(const WebViewManager&) = delete;

//loading libraries-headers
bool loadLibrary (std::wstring lib_load);

bool resolveFunctions ();








};