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

Passwordinfo passinfo("1234" , "test" , "D:/test.txt" ,"Test-title");
{
cout << "File Path: " << passinfo.get_filepath() << '\n'
     << "File Name: " << passinfo.get_filename() << '\n'
     << "File Exists: " << passinfo.file_exists() << '\n';
}

cout << "Is File: " << passinfo.is_file() << '\n';
cout << "Is Folder: " << passinfo.is_folder() << '\n';

   return  app.exec() ; 
}