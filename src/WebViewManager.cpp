
#include "WebViewManager.h"

#include <iostream>
#include <new>

// Environment Handler

EnvironmentHandler::EnvironmentHandler(WebViewManager* manager)
    : owner(manager) {}

HRESULT STDMETHODCALLTYPE EnvironmentHandler::QueryInterface(
    REFIID riid, void** object)
{
    if (!object)
        return E_POINTER;

    *object = nullptr;

    if (IsEqualIID(riid, IID_IUnknown) ||
        IsEqualIID(
            riid,
            IID_ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler))
    {
        *object = static_cast<
            ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*
        >(this);

        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE EnvironmentHandler::AddRef()
{
    return static_cast<ULONG>(InterlockedIncrement(&refCount));
}

ULONG STDMETHODCALLTYPE EnvironmentHandler::Release()
{
    ULONG count =
        static_cast<ULONG>(InterlockedDecrement(&refCount));

    if (count == 0)
        delete this;

    return count;
}

HRESULT STDMETHODCALLTYPE EnvironmentHandler::Invoke(
    HRESULT errorCode,
    ICoreWebView2Environment* createdEnvironment)
{
    owner->onEnvironmentCreated(errorCode, createdEnvironment);
    return S_OK;
}

// Controller Handler

ControllerHandler::ControllerHandler(WebViewManager* manager)
    : owner(manager) {}

HRESULT STDMETHODCALLTYPE ControllerHandler::QueryInterface(
    REFIID riid, void** object)
{
    if (!object)
        return E_POINTER;

    *object = nullptr;

    if (IsEqualIID(riid, IID_IUnknown) ||
        IsEqualIID(
            riid,
            IID_ICoreWebView2CreateCoreWebView2ControllerCompletedHandler))
    {
        *object = static_cast<
            ICoreWebView2CreateCoreWebView2ControllerCompletedHandler*
        >(this);

        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE ControllerHandler::AddRef()
{
    return static_cast<ULONG>(InterlockedIncrement(&refCount));
}

ULONG STDMETHODCALLTYPE ControllerHandler::Release()
{
    ULONG count =
        static_cast<ULONG>(InterlockedDecrement(&refCount));

    if (count == 0)
        delete this;

    return count;
}

HRESULT STDMETHODCALLTYPE ControllerHandler::Invoke(
    HRESULT errorCode,
    ICoreWebView2Controller* createdController)
{
    owner->onControllerCreated(errorCode, createdController);
    return S_OK;
}

// WebViewManager

bool WebViewManager::loadLibrary(const std::wstring& path)
{
    if (lib_handler)
        return true;

    lib_handler = LoadLibraryW(path.c_str());

    return lib_handler != nullptr;
}

bool WebViewManager::resolveFunctions()
{
    if (!lib_handler)
        return false;

    auto address = GetProcAddress(
        lib_handler,
        "CreateCoreWebView2EnvironmentWithOptions"
    );

    func_address =
        reinterpret_cast<CreateEnvironmentFn>(address);

    return func_address != nullptr;
}

bool WebViewManager::createEnvironment(HWND parent)
{
    if (!func_address || !IsWindow(parent))
        return false;

    parentWindow = parent;

    auto* handler = new (std::nothrow) EnvironmentHandler(this);

    if (!handler)
        return false;

    HRESULT result = func_address(
        nullptr,
        nullptr,
        nullptr,
        handler
    );

    handler->Release();

    if (FAILED(result))
    {
        std::cout << "Environment request failed: 0x"
                  << std::hex
                  << static_cast<unsigned long>(result)
                  << std::dec << '\n';

        return false;
    }

    std::cout << "Environment request accepted.\n";
    return true;
}

void WebViewManager::onEnvironmentCreated(
    HRESULT result,
    ICoreWebView2Environment* env)
{
    if (FAILED(result) || !env)
    {
        std::cout << "Environment creation failed: 0x"
                  << std::hex
                  << static_cast<unsigned long>(result)
                  << std::dec << '\n';
        return;
    }

    std::cout << "Environment created successfully.\n";

    environment = env;
    environment->AddRef();

    auto* handler = new (std::nothrow) ControllerHandler(this);

    if (!handler)
    {
        std::cout << "Cannot allocate ControllerHandler.\n";
        return;
    }

    HRESULT hr = environment->CreateCoreWebView2Controller(
        parentWindow,
        handler
    );

    handler->Release();

    if (FAILED(hr))
    {
        std::cout << "Controller request failed: 0x"
                  << std::hex
                  << static_cast<unsigned long>(hr)
                  << std::dec << '\n';
    }
}

void WebViewManager::onControllerCreated(
    HRESULT result,
    ICoreWebView2Controller* ctrl)
{
    if (FAILED(result) || !ctrl)
    {
        std::cout << "Controller creation failed: 0x"
                  << std::hex
                  << static_cast<unsigned long>(result)
                  << std::dec << '\n';
        return;
    }

    controller = ctrl;
    controller->AddRef();

    HRESULT hr = controller->get_CoreWebView2(&webview);

    if (FAILED(hr) || !webview)
    {
        std::cout << "Cannot get CoreWebView2.\n";
        return;
    }

    resize();

    const wchar_t* html = LR"HTML(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>GX WebView2 Test</title>
    <style>
        body {
            background: #101827;
            color: #f1f5f9;
            font-family: Arial, sans-serif;
            text-align: center;
            padding: 80px 20px;
        }
        h1 {
            color: #38bdf8;
        }
        .panel {
            background: #1e293b;
            padding: 30px;
            border-radius: 14px;
            max-width: 500px;
            margin: auto;
        }
        button {
            background: #38bdf8;
            color: #101827;
            padding: 12px 24px;
            border: 0;
            border-radius: 8px;
            cursor: pointer;
        }
    </style>
</head>
<body>
    <div class="panel">
        <h1>GX Desktop</h1>
        <p>WebView2 + Qt + C++17</p>
        <p id="status">HTML loaded successfully.</p>
        <button onclick="document.getElementById('status').textContent =
            'JavaScript is working!'">
            Test JavaScript
        </button>
    </div>
</body>
</html>
)HTML";

    hr = webview->NavigateToString(html);

    if (FAILED(hr))
    {
        std::cout << "HTML navigation failed: 0x"
                  << std::hex
                  << static_cast<unsigned long>(hr)
                  << std::dec << '\n';
        return;
    }

    std::cout << "HTML navigation requested.\n";
}

void WebViewManager::resize()
{
    if (!controller || !IsWindow(parentWindow))
        return;

    RECT bounds{};

    if (GetClientRect(parentWindow, &bounds))
        controller->put_Bounds(bounds);
}

WebViewManager::~WebViewManager()
{
    if (controller)
    {
        controller->Close();
    }

    if (webview)
    {
        webview->Release();
        webview = nullptr;
    }

    if (controller)
    {
        controller->Release();
        controller = nullptr;
    }

    if (environment)
    {
        environment->Release();
        environment = nullptr;
    }

    if (lib_handler)
    {
        FreeLibrary(lib_handler);
        lib_handler = nullptr;
    }
}
