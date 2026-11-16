
#pragma once

#include <windows.h>
#include <WebView2.h>
#include <string>

using CreateEnvironmentFn = HRESULT (STDAPICALLTYPE *)(
    PCWSTR,
    PCWSTR,
    ICoreWebView2EnvironmentOptions*,
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*
);

class WebViewManager;

class EnvironmentHandler final
    : public ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler
{
private:
    volatile LONG refCount = 1;
    WebViewManager* owner;

public:
    explicit EnvironmentHandler(WebViewManager* manager);

    HRESULT STDMETHODCALLTYPE QueryInterface(
        REFIID riid, void** object) override;

    ULONG STDMETHODCALLTYPE AddRef() override;
    ULONG STDMETHODCALLTYPE Release() override;

    HRESULT STDMETHODCALLTYPE Invoke(
        HRESULT errorCode,
        ICoreWebView2Environment* environment) override;
};

class ControllerHandler final
    : public ICoreWebView2CreateCoreWebView2ControllerCompletedHandler
{
private:
    volatile LONG refCount = 1;
    WebViewManager* owner;

public:
    explicit ControllerHandler(WebViewManager* manager);

    HRESULT STDMETHODCALLTYPE QueryInterface(
        REFIID riid, void** object) override;

    ULONG STDMETHODCALLTYPE AddRef() override;
    ULONG STDMETHODCALLTYPE Release() override;

    HRESULT STDMETHODCALLTYPE Invoke(
        HRESULT errorCode,
        ICoreWebView2Controller* controller) override;
};

class WebViewManager
{
private:
    HMODULE lib_handler = nullptr;
    CreateEnvironmentFn func_address = nullptr;

    HWND parentWindow = nullptr;

    ICoreWebView2Environment* environment = nullptr;
    ICoreWebView2Controller* controller = nullptr;
    ICoreWebView2* webview = nullptr;

public:
    WebViewManager() = default;
    ~WebViewManager();

    WebViewManager(const WebViewManager&) = delete;
    WebViewManager& operator=(const WebViewManager&) = delete;

    bool loadLibrary(const std::wstring& lib_load);
    bool resolveFunctions();
    bool createEnvironment(HWND parent);

    void onEnvironmentCreated(
        HRESULT result,
        ICoreWebView2Environment* env);

    void onControllerCreated(
        HRESULT result,
        ICoreWebView2Controller* ctrl);

    void resize();
};
