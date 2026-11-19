#include <sqlite3.h>

#include <iostream>

using namespace std;

int main() {
  sqlite3* db = nullptr;

  int result = sqlite3_open("gx_test.db", &db);

  if (result != SQLITE_OK) {
    cout << "Database connection failed: " << sqlite3_errmsg(db) << "\n";

    if (db != nullptr) {
      sqlite3_close(db);
    }

    return 1;
  }

  cout << "Database opened successfully\n";

  const char* sql =
      "CREATE TABLE IF NOT EXISTS records ("
      "id INTEGER PRIMARY KEY, "
      "title TEXT NOT NULL CHECK(length(trim(title)) > 0), "
      "notes TEXT, "
      "filepath TEXT, "
      "created_at TEXT NOT NULL, "
      "protection_method TEXT, "
      "status TEXT NOT NULL DEFAULT 'active' "
      "CHECK(status IN ('active', 'locked', 'archived'))"
      ");";

  int execResult = sqlite3_exec(db, sql, nullptr, nullptr, nullptr);

  if (execResult == SQLITE_OK) {
    cout << "Table schema initialized successfully\n";
  } else {
    cout << "Table creation failed: " << sqlite3_errmsg(db) << "\n";
  }

  int closeResult = sqlite3_close(db);

  if (closeResult == SQLITE_OK) {
    cout << "Database closed successfully\n";
  } else {
    cout << "Database close failed\n";
    return 1;
  }

  return execResult == SQLITE_OK ? 0 : 1;
}
