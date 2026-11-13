#include <iostream> 
#include <string> 
#include <QApplication>
#include <QMainWindow>
#include "PasswordManager.h"
#include "GXRecordManager.h"


using namespace std;

int main(int argc , char* argv[]){

QApplication app (argc , argv );
QMainWindow window ; 

window.show(); 






Passwordinfo file("1234", "Test", "C:/Windows/notepad.exe", "File");
Passwordinfo folder("1234", "Test", "C:/Windows", "Folder");
Passwordinfo missing("1234", "Test", "C:/GX_TEST_NONEXISTENT_12345.txt", "Missing");

std::cout << std::boolalpha;

std::cout << "\n--- FILE ---\n";
std::cout << file.file_exists() << " "
          << file.is_file() << " "
          << file.is_folder() << '\n';

std::cout << "\n--- FOLDER ---\n";
std::cout << folder.file_exists() << " "
          << folder.is_file() << " "
          << folder.is_folder() << '\n';

std::cout << "\n--- MISSING ---\n";
std::cout << missing.file_exists() << " "
          << missing.is_file() << " "
          << missing.is_folder() << '\n';



GXRecordManager manager;

std::cout << "Initial Records: "
          << manager.getRecordCount() << '\n';

Passwordinfo pass1("1234", "Test 1", "D:/file1.txt", "File 1");
Passwordinfo pass2("5678", "Test 2", "D:/file2.txt", "File 2");

manager.add_record(pass1);
manager.add_record(pass2);
std::cout << std::boolalpha;

std::cout << "\n--- Before Removal ---\n";
std::cout << "Records: " << manager.getRecordCount() << '\n';

std::cout << "\n--- Remove Existing Record ---\n";
bool result1 = manager.remove_record(pass1.get_id());
std::cout << "Removed: " << result1 << '\n';

std::cout << "\n--- Remove Non-existing Record ---\n";
bool result2 = manager.remove_record("NOT_FOUND_123");
std::cout << "Removed: " << result2 << '\n';

std::cout << "\n--- After Removal ---\n";
std::cout << "Records: " << manager.getRecordCount() << '\n';

std::cout << "Records After Adding: "
          << manager.getRecordCount() << '\n';


std::cout << "\n--- Test 1: Existing Record ---\n";
manager.find_record(pass1.get_id());

std::cout << "\n--- Test 2: Non-existing Record ---\n";
manager.find_record("NOT_FOUND_123");


   return  app.exec() ; 
}