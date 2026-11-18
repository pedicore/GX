
#include "WebViewManager.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QString>
#include <iostream>
#include <new>
#include <string>
#include <utility>

namespace {
HRESULT sendJsonResponse(ICoreWebView2* sender, const QJsonObject& response) {
  if (!sender) return E_POINTER;
  const QByteArray bytes =
      QJsonDocument(response).toJson(QJsonDocument::Compact);
  const std::wstring wide = QString::fromUtf8(bytes).toStdWString();
  return sender->PostWebMessageAsJson(wide.c_str());
}

QJsonObject errorResponse(const QString& requestId, const QString& error) {
  return QJsonObject{{"type", "bridge.error"},
                     {"requestId", requestId},
                     {"success", false},
                     {"error", error}};
}

HRESULT processMessage(ICoreWebView2* sender,
                       ICoreWebView2WebMessageReceivedEventArgs* args) {
  if (!sender || !args) return E_POINTER;
  LPWSTR raw = nullptr;
  const HRESULT readHr = args->get_WebMessageAsJson(&raw);
  if (FAILED(readHr)) return readHr;
  if (!raw) return E_FAIL;

  // Copy into Qt-owned storage, then immediately free COM-owned memory.
  const QString jsonText = QString::fromWCharArray(raw);
  CoTaskMemFree(raw);

  std::cout << "Message from JavaScript: " << jsonText.toStdString() << '\n';

  QJsonParseError parseError;
  const QJsonDocument doc =
      QJsonDocument::fromJson(jsonText.toUtf8(), &parseError);
  QJsonObject response;

  if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
    response = errorResponse("", "Invalid JSON request");
  } else {
    const QJsonObject request = doc.object();
    const auto typeValue = request.value("type");
    const auto idValue = request.value("requestId");
    const auto payloadValue = request.value("payload");

    const QString type = typeValue.toString();
    const QString requestId = idValue.toString();
    const bool validType = typeValue.isString() && !type.trimmed().isEmpty();
    const bool validId = idValue.isString() && !requestId.trimmed().isEmpty();
    const bool validPayload = payloadValue.isObject();
    const QString responseId = validId ? requestId : QString();

    if (!validType || !validId || !validPayload) {
      std::cout << "Request structure is invalid.\n";
      response = errorResponse(responseId, "Invalid request structure");
    } else if (type == "bridge.ping") {
      std::cout << "Valid bridge.ping request: " << requestId.toStdString()
                << '\n';
      response = QJsonObject{
          {"type", "bridge.ping.result"},
          {"requestId", requestId},
          {"success", true},
          {"data", QJsonObject{{"message", "GX Backend Connected!"}}}};
    } else {
      std::cout << "Unsupported request type: " << type.toStdString() << '\n';
      response = errorResponse(requestId, "Unsupported request type");
    }
  }

  const HRESULT sendHr = sendJsonResponse(sender, response);
  if (FAILED(sendHr)) std::cout << "Failed to send JSON response.\n";
  return sendHr;
}
}  // namespace

EnvironmentHandler::EnvironmentHandler(std::shared_ptr<WebViewCallbackState> s)
    : state(std::move(s)) {}
HRESULT STDMETHODCALLTYPE EnvironmentHandler::QueryInterface(REFIID riid,
                                                             void** object) {
  if (!object) return E_POINTER;
  *object = nullptr;
  if (IsEqualIID(riid, IID_IUnknown) ||
      IsEqualIID(
          riid,
          IID_ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler)) {
    *object = static_cast<
        ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*>(this);
    AddRef();
    return S_OK;
  }
  return E_NOINTERFACE;
}
ULONG STDMETHODCALLTYPE EnvironmentHandler::AddRef() {
  return static_cast<ULONG>(InterlockedIncrement(&refCount));
}
ULONG STDMETHODCALLTYPE EnvironmentHandler::Release() {
  const ULONG count = static_cast<ULONG>(InterlockedDecrement(&refCount));
  if (!count) delete this;
  return count;
}
HRESULT STDMETHODCALLTYPE
EnvironmentHandler::Invoke(HRESULT errorCode, ICoreWebView2Environment* env) {
  if (state->owner) state->owner->onEnvironmentCreated(errorCode, env);
  return S_OK;
}

ControllerHandler::ControllerHandler(std::shared_ptr<WebViewCallbackState> s)
    : state(std::move(s)) {}
HRESULT STDMETHODCALLTYPE ControllerHandler::QueryInterface(REFIID riid,
                                                            void** object) {
  if (!object) return E_POINTER;
  *object = nullptr;
  if (IsEqualIID(riid, IID_IUnknown) ||
      IsEqualIID(
          riid,
          IID_ICoreWebView2CreateCoreWebView2ControllerCompletedHandler)) {
    *object =
        static_cast<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler*>(
            this);
    AddRef();
    return S_OK;
  }
  return E_NOINTERFACE;
}
ULONG STDMETHODCALLTYPE ControllerHandler::AddRef() {
  return static_cast<ULONG>(InterlockedIncrement(&refCount));
}
ULONG STDMETHODCALLTYPE ControllerHandler::Release() {
  const ULONG count = static_cast<ULONG>(InterlockedDecrement(&refCount));
  if (!count) delete this;
  return count;
}
HRESULT STDMETHODCALLTYPE
ControllerHandler::Invoke(HRESULT errorCode, ICoreWebView2Controller* ctrl) {
  if (state->owner) state->owner->onControllerCreated(errorCode, ctrl);
  return S_OK;
}

WebMessageHandler::WebMessageHandler(std::shared_ptr<WebViewCallbackState> s)
    : state(std::move(s)) {}
HRESULT STDMETHODCALLTYPE WebMessageHandler::QueryInterface(REFIID riid,
                                                            void** object) {
  if (!object) return E_POINTER;
  *object = nullptr;
  if (IsEqualIID(riid, IID_IUnknown) ||
      IsEqualIID(riid, IID_ICoreWebView2WebMessageReceivedEventHandler)) {
    *object = static_cast<ICoreWebView2WebMessageReceivedEventHandler*>(this);
    AddRef();
    return S_OK;
  }
  return E_NOINTERFACE;
}
ULONG STDMETHODCALLTYPE WebMessageHandler::AddRef() {
  return static_cast<ULONG>(InterlockedIncrement(&refCount));
}
ULONG STDMETHODCALLTYPE WebMessageHandler::Release() {
  const ULONG count = static_cast<ULONG>(InterlockedDecrement(&refCount));
  if (!count) delete this;
  return count;
}
HRESULT STDMETHODCALLTYPE WebMessageHandler::Invoke(
    ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args) {
  if (!state->owner) return S_OK;
  return processMessage(sender, args);
}

WebViewManager::WebViewManager()
    : callbackState(std::make_shared<WebViewCallbackState>()) {
  callbackState->owner = this;
}

bool WebViewManager::loadLibrary(const std::wstring& path) {
  if (lib_handler) return true;
  lib_handler = LoadLibraryW(path.c_str());
  if (!lib_handler) std::cout << "Failed to load WebView2 loader DLL.\n";
  return lib_handler != nullptr;
}

bool WebViewManager::resolveFunctions() {
  if (!lib_handler) return false;
  func_address = reinterpret_cast<CreateEnvironmentFn>(
      GetProcAddress(lib_handler, "CreateCoreWebView2EnvironmentWithOptions"));
  return func_address != nullptr;
}

bool WebViewManager::createEnvironment(HWND parent) {
  if (!func_address || !IsWindow(parent)) return false;
  parentWindow = parent;
  auto* handler = new (std::nothrow) EnvironmentHandler(callbackState);
  if (!handler) return false;
  const HRESULT hr = func_address(nullptr, nullptr, nullptr, handler);
  handler->Release();
  if (FAILED(hr)) {
    std::cout << "Environment request failed: 0x" << std::hex
              << static_cast<unsigned long>(hr) << std::dec << '\n';
    return false;
  }
  std::cout << "Environment request accepted.\n";
  return true;
}

void WebViewManager::onEnvironmentCreated(HRESULT result,
                                          ICoreWebView2Environment* env) {
  if (FAILED(result) || !env) {
    std::cout << "Environment creation failed.\n";
    return;
  }
  std::cout << "Environment created successfully.\n";
  environment = env;
  environment->AddRef();
  auto* handler = new (std::nothrow) ControllerHandler(callbackState);
  if (!handler) {
    std::cout << "Cannot allocate ControllerHandler.\n";
    return;
  }
  const HRESULT hr =
      environment->CreateCoreWebView2Controller(parentWindow, handler);
  handler->Release();
  if (FAILED(hr)) std::cout << "Controller request failed.\n";
}

void WebViewManager::onControllerCreated(HRESULT result,
                                         ICoreWebView2Controller* ctrl) {
  if (FAILED(result) || !ctrl) {
    std::cout << "Controller creation failed.\n";
    return;
  }
  controller = ctrl;
  controller->AddRef();
  const HRESULT hr = controller->get_CoreWebView2(&webview);
  if (FAILED(hr) || !webview) {
    std::cout << "Cannot get CoreWebView2.\n";
    return;
  }

  auto* handler = new (std::nothrow) WebMessageHandler(callbackState);
  if (!handler) {
    std::cout << "Cannot allocate WebMessageHandler.\n";
    return;
  }
  const HRESULT registerHr =
      webview->add_WebMessageReceived(handler, &messageToken);
  handler->Release();
  if (FAILED(registerHr)) {
    std::cout << "Failed to register WebMessageReceived.\n";
    return;
  }
  messageRegistered = true;
  std::cout << "WebMessageReceived handler registered.\n";
  resize();

  const wchar_t* html =
      LR"GXHTML(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>GX WebView2 Bridge</title>
<style>
  body { background:#101827; color:#f1f5f9; font-family:Arial,sans-serif;
         text-align:center; padding:80px 20px; }
  h1 { color:#38bdf8; }
  .panel { background:#1e293b; padding:30px; border-radius:14px;
           max-width:500px; margin:auto; }
  button { background:#38bdf8; color:#101827; padding:12px 24px;
           border:0; border-radius:8px; cursor:pointer; }
  #status { margin:24px 0; overflow-wrap:anywhere; }
</style>
</head>
<body>
<div class="panel">
  <h1>GX Desktop</h1>
  <p>WebView2 + Qt + C++17</p>
  <p id="status">HTML loaded successfully.</p>
  <button onclick="sendTestRequest()">Test JSON Connection</button>
</div>
<script>
  let nextRequestId = 1;
  let pendingRequestId = null;
  function sendTestRequest() {
    if (!window.chrome?.webview) {
      document.getElementById('status').textContent = 'WebView2 bridge unavailable';
      return;
    }
    const request = {
      type: 'bridge.ping',
      requestId: 'req-' + nextRequestId++,
      payload: { message: 'Hello from GX Frontend!' }
    };
    pendingRequestId = request.requestId;
    document.getElementById('status').textContent = 'Waiting for backend response...';
    window.chrome.webview.postMessage(request);
  }
  window.chrome.webview.addEventListener('message', function(event) {
    const response = event.data;
    const status = document.getElementById('status');
    if (!response || typeof response !== 'object') {
      status.textContent = 'Invalid backend response';
      return;
    }
    if (response.requestId !== pendingRequestId) {
      status.textContent = 'Unexpected response ID';
      return;
    }
    if (response.type === 'bridge.ping.result' && response.success === true &&
        typeof response.data?.message === 'string') {
      status.textContent = response.data.message;
    } else if (response.type === 'bridge.error' && response.success === false) {
      status.textContent = typeof response.error === 'string'
        ? response.error : 'Unknown backend error';
    } else {
      status.textContent = 'Unexpected backend response';
    }
    pendingRequestId = null;
  });
</script>
</body>
</html>
)GXHTML";

  const HRESULT navigateHr = webview->NavigateToString(html);
  if (FAILED(navigateHr)) {
    std::cout << "HTML navigation failed.\n";
    return;
  }
  std::cout << "HTML navigation requested.\n";
}

void WebViewManager::resize() {
  if (!controller || !IsWindow(parentWindow)) return;
  RECT bounds{};
  if (GetClientRect(parentWindow, &bounds)) controller->put_Bounds(bounds);
}

WebViewManager::~WebViewManager() {
  // Use this manager on the same UI/COM apartment thread as its callbacks.
  callbackState->owner = nullptr;
  if (webview && messageRegistered) {
    webview->remove_WebMessageReceived(messageToken);
    messageRegistered = false;
  }
  if (controller) controller->Close();
  if (webview) {
    webview->Release();
    webview = nullptr;
  }
  if (controller) {
    controller->Release();
    controller = nullptr;
  }
  if (environment) {
    environment->Release();
    environment = nullptr;
  }
  if (lib_handler) {
    FreeLibrary(lib_handler);
    lib_handler = nullptr;
  }
}
