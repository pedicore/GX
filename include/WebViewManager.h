
#pragma once

#include <WebView2.h>
#include <windows.h>

#include <memory>
#include <string>

using CreateEnvironmentFn = HRESULT(STDAPICALLTYPE*)(
    PCWSTR, PCWSTR, ICoreWebView2EnvironmentOptions*,
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*);

class WebViewManager;

// Callbacks may finish after shutdown begins. They check this shared state
// before accessing the manager.
struct WebViewCallbackState {
  WebViewManager* owner = nullptr;
};

class EnvironmentHandler final
    : public ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler {
  volatile LONG refCount = 1;
  std::shared_ptr<WebViewCallbackState> state;

 public:
  explicit EnvironmentHandler(std::shared_ptr<WebViewCallbackState> state);
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** object) override;
  ULONG STDMETHODCALLTYPE AddRef() override;
  ULONG STDMETHODCALLTYPE Release() override;
  HRESULT STDMETHODCALLTYPE
  Invoke(HRESULT errorCode, ICoreWebView2Environment* environment) override;
};

class ControllerHandler final
    : public ICoreWebView2CreateCoreWebView2ControllerCompletedHandler {
  volatile LONG refCount = 1;
  std::shared_ptr<WebViewCallbackState> state;

 public:
  explicit ControllerHandler(std::shared_ptr<WebViewCallbackState> state);
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** object) override;
  ULONG STDMETHODCALLTYPE AddRef() override;
  ULONG STDMETHODCALLTYPE Release() override;
  HRESULT STDMETHODCALLTYPE
  Invoke(HRESULT errorCode, ICoreWebView2Controller* controller) override;
};

class WebMessageHandler final
    : public ICoreWebView2WebMessageReceivedEventHandler {
  volatile LONG refCount = 1;
  std::shared_ptr<WebViewCallbackState> state;

 public:
  explicit WebMessageHandler(std::shared_ptr<WebViewCallbackState> state);
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** object) override;
  ULONG STDMETHODCALLTYPE AddRef() override;
  ULONG STDMETHODCALLTYPE Release() override;
  HRESULT STDMETHODCALLTYPE
  Invoke(ICoreWebView2* sender,
         ICoreWebView2WebMessageReceivedEventArgs* args) override;
};

class WebViewManager {
  HMODULE lib_handler = nullptr;
  CreateEnvironmentFn func_address = nullptr;
  HWND parentWindow = nullptr;
  ICoreWebView2Environment* environment = nullptr;
  ICoreWebView2Controller* controller = nullptr;
  ICoreWebView2* webview = nullptr;
  EventRegistrationToken messageToken{};
  bool messageRegistered = false;
  std::shared_ptr<WebViewCallbackState> callbackState;

 public:
  WebViewManager();
  ~WebViewManager();
  WebViewManager(const WebViewManager&) = delete;
  WebViewManager& operator=(const WebViewManager&) = delete;

  bool loadLibrary(const std::wstring& path);
  bool resolveFunctions();
  bool createEnvironment(HWND parent);
  void onEnvironmentCreated(HRESULT result, ICoreWebView2Environment* env);
  void onControllerCreated(HRESULT result, ICoreWebView2Controller* ctrl);
  void resize();
};
