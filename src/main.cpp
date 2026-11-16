#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>

#include <windows.h>
#include <iostream>

#include "WebViewManager.h"

int main(int argc, char* argv[])
{
    HRESULT comResult = CoInitializeEx(
        nullptr,
        COINIT_APARTMENTTHREADED
    );

    if (FAILED(comResult))
    {
        std::cout << "COM initialization failed: 0x"
                  << std::hex
                  << static_cast<unsigned long>(comResult)
                  << std::dec << '\n';
        return 1;
    }

    int exitCode = 1;

    {
        QApplication app(argc, argv);

        QWidget window;
        window.setWindowTitle("GX - WebView2 Test");
        window.resize(900, 600);

        auto* layout = new QVBoxLayout(&window);
        layout->setContentsMargins(0, 0, 0, 0);

        QWidget browserHost;
        layout->addWidget(&browserHost);

        window.show();

        WebViewManager web_view;

        if (!web_view.loadLibrary(L"WebView2Loader.dll"))
        {
            std::cout << "Cannot load WebView2Loader.dll\n";
        }
        else if (!web_view.resolveFunctions())
        {
            std::cout << "Cannot resolve WebView2 function\n";
        }
        else if (!web_view.createEnvironment(
                     reinterpret_cast<HWND>(browserHost.winId())))
        {
            std::cout << "Cannot start WebView2\n";
        }

        exitCode = app.exec();
    }

    CoUninitialize();

    return exitCode;
}
