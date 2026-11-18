#include <windows.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QWidget>
#include <iostream>

#include "GXRecordManager.h"
#include "PasswordManager.h"
#include "WebViewManager.h"

int main(int argc, char* argv[]) {
  HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

  if (FAILED(comResult)) {
    std::cout << "COM initialization failed: 0x" << std::hex
              << static_cast<unsigned long>(comResult) << std::dec << '\n';
    return 1;
  }

  int exitCode = 1;

  {
    QApplication app(argc, argv);

    // Day 6 - GXRecordManager Test

    GXRecordManager manager;

    PasswordManager record1("test_pass_1", "GitHub account", "", "GitHub");

    PasswordManager record2("test_pass_2", "Email account", "", "Gmail");

    PasswordManager record3("test_pass_3", "Discord account", "", "Discord");

    manager.add_record(record1);
    manager.add_record(record2);
    manager.add_record(record3);

    std::cout << "\n=== GX Day 6 Test ===\n";

    std::cout << "Total records: " << manager.getRecordCount() << '\n';

    //
    std::cout << "\n=== Find by ID Test ===\n";

    std::cout << "GitHub ID: " << record1.get_id() << '\n';
    std::cout << "Gmail ID: " << record2.get_id() << '\n';
    std::cout << "Discord ID: " << record3.get_id() << '\n';

    manager.find_record(record1.get_id());
    manager.find_record(record2.get_id());
    manager.find_record(record3.get_id());

    std::cout << "\n=== Update Record Test ===\n";
    //
    bool updated = manager.update_record(record1.get_id(), "GitHub - Updated");

    std::cout << "Update successful: " << std::boolalpha << updated << '\n';

    manager.find_record(record1.get_id());
    //
    std::cout << "\n=== Remove Record Test ===\n";

    bool removed = manager.remove_record(record2.get_id());

    std::cout << "Remove successful: " << std::boolalpha << removed << '\n';

    std::cout << "Total records after removal: " << manager.getRecordCount()
              << '\n';

    std::cout << "\nSearching for deleted record:\n";
    manager.find_record(record2.get_id());

    //
    bool removedAgain = manager.remove_record(record2.get_id());

    std::cout << "Remove again: " << std::boolalpha << removedAgain << '\n';

    std::cout << "Final record count: " << manager.getRecordCount() << '\n';

    QWidget window;
    window.setWindowTitle("GX - WebView2 Test");
    window.resize(900, 600);

    auto* layout = new QVBoxLayout(&window);
    layout->setContentsMargins(0, 0, 0, 0);

    QWidget browserHost;
    layout->addWidget(&browserHost);

    window.show();

    WebViewManager web_view;

    if (!web_view.loadLibrary(L"WebView2Loader.dll")) {
      std::cout << "Cannot load WebView2Loader.dll\n";
    } else if (!web_view.resolveFunctions()) {
      std::cout << "Cannot resolve WebView2 function\n";
    } else if (!web_view.createEnvironment(
                   reinterpret_cast<HWND>(browserHost.winId()))) {
      std::cout << "Cannot start WebView2\n";
    }

    exitCode = app.exec();
  }

  CoUninitialize();

  return exitCode;
}
