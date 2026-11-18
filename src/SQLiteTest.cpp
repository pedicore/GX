#include <sqlite3.h>

#include <iostream>

using namespace std;

int main() {
  sqlite3* db = nullptr;
  int result = sqlite3_open("gx_test.db", &db);
  if (result == SQLITE_OK) {
    std::cout << "Database opened successfully\n";
  } else {
    std::cout << "Database connection failed\n";
  }

  if (db != nullptr) {
    int CloseResult = sqlite3_close(db);
    if (CloseResult == SQLITE_OK) {
      std::cout << "Database closed successfully\n";
    } else {
      std::cout << "Database close failed\n";
    }
  }

  return 0;
}
