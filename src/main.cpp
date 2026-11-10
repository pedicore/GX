#include <iostream> 
#include <string> 
#include <QApplication>
#include <QMainWindow>
#include "PasswordManager.h"

using namespace std;

int main(int argc , char* argv[]){

QApplication app (argc , argv );
QMainWindow window ; 

window.show(); 

PassWordinfo file1(
    "test123",
    "My personal documents",
    "C:\\Users\\Test\\Documents",
    "Documents"
);
PassWordinfo file2(
    "test456",
    "My personal documents -2",
    "C:\\Users\\Test\\Documents",
    "Documents"
);

{
std::cout << file1.get_title() << '\n';
std::cout << file1.get_id() << '\n';
std::cout << file1.get_date() << '\n';

}
{
std::cout << file2.get_title() << '\n';
std::cout << file2.get_id() << '\n';
std::cout << file2.get_date() << '\n';
}



   return  app.exec() ; 
}