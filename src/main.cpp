#include <iostream> 
#include <string> 
#include <QApplication>
#include <QMainWindow>
#include "PasswordManager.h"
#include "GXRecordManager.h"
#include "WebViewManager.h"


using namespace std;

int main(int argc , char* argv[]){

QApplication app (argc , argv );
QMainWindow window ; 
GXRecordManager manager;
WebViewManager web_view;
window.show(); 


cout<<web_view.loadLibrary(L"WebView2Loader.dll")<<"\n";
cout<<web_view.resolveFunctions()<<"\n";


PasswordManager file("1234", "Test", "C:/Windows/notepad.exe", "File");
PasswordManager folder("1234", "Test", "C:/Windows", "Folder");
PasswordManager missing("1234", "Test", "C:/GX_TEST_NONEXISTENT_12345.txt", "Missing");





std::cout << "Initial Records: "
          << manager.getRecordCount() << '\n';

PasswordManager pass1("1234", "Test 1", "D:/file1.txt", "File 1");
PasswordManager pass2("5678", "Test 2", "D:/file2.txt", "File 2");

manager.add_record(pass1);
manager.add_record(pass2);








   return  app.exec(); 
}