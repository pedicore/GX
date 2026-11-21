
#include <sqlite3.h>

#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

struct Record {
  int id = 0;
  string title;
  string notes;
  string filepath;
  string created_at;
  string protection_method;
  string status = "active";
};

class GXDatabase {
 private:
  sqlite3* db = nullptr;

  // Automatically releases a prepared statement.
  struct Statement {
    sqlite3_stmt* ptr = nullptr;

    ~Statement() {
      if (ptr) sqlite3_finalize(ptr);
    }

    Statement(const Statement&) = delete;
    Statement& operator=(const Statement&) = delete;

    Statement() = default;
  };

  void check(int result, int expected) {
    if (result != expected) {
      throw runtime_error(sqlite3_errmsg(db));
    }
  }

  void prepare(Statement& stmt, const char* sql) {
    check(sqlite3_prepare_v2(db, sql, -1, &stmt.ptr, nullptr), SQLITE_OK);
  }

  void bindText(sqlite3_stmt* stmt, int index, const string& value) {
    check(sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT),
          SQLITE_OK);
  }

  static string columnText(sqlite3_stmt* stmt, int index) {
    const unsigned char* value = sqlite3_column_text(stmt, index);

    return value ? reinterpret_cast<const char*>(value) : "";
  }

  static Record readRecord(sqlite3_stmt* stmt) {
    Record r;
    r.id = sqlite3_column_int(stmt, 0);
    r.title = columnText(stmt, 1);
    r.notes = columnText(stmt, 2);
    r.filepath = columnText(stmt, 3);
    r.created_at = columnText(stmt, 4);
    r.protection_method = columnText(stmt, 5);
    r.status = columnText(stmt, 6);
    return r;
  }

 public:
  explicit GXDatabase(const string& path) {
    int rc = sqlite3_open(path.c_str(), &db);

    if (rc != SQLITE_OK) {
      string error =
          db ? sqlite3_errmsg(db) : "Cannot allocate SQLite connection";

      if (db) sqlite3_close(db);
      db = nullptr;
      throw runtime_error(error);
    }
  }

  ~GXDatabase() {
    if (db) sqlite3_close(db);
  }

  GXDatabase(const GXDatabase&) = delete;
  GXDatabase& operator=(const GXDatabase&) = delete;

  void initialize() {
    const char* sql = R"SQL(
            CREATE TABLE IF NOT EXISTS records (
                id INTEGER PRIMARY KEY,
                title TEXT NOT NULL
                    CHECK(length(trim(title)) > 0),
                notes TEXT,
                filepath TEXT,
                created_at TEXT NOT NULL,
                protection_method TEXT,
                status TEXT NOT NULL DEFAULT 'active'
                    CHECK(status IN (
                        'active', 'locked', 'archived'
                    ))
            );
        )SQL";

    char* error = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &error);

    if (rc != SQLITE_OK) {
      string message = error ? error : "Schema initialization failed";

      sqlite3_free(error);
      throw runtime_error(message);
    }
  }

  // CREATE: Insert a new record.
  int addRecord(const Record& r) {
    const char* sql = R"SQL(
            INSERT INTO records
            (title, notes, filepath, created_at,
             protection_method, status)
            VALUES (?, ?, ?, ?, ?, ?);
        )SQL";

    Statement stmt;
    prepare(stmt, sql);

    bindText(stmt.ptr, 1, r.title);
    bindText(stmt.ptr, 2, r.notes);
    bindText(stmt.ptr, 3, r.filepath);
    bindText(stmt.ptr, 4, r.created_at);
    bindText(stmt.ptr, 5, r.protection_method);
    bindText(stmt.ptr, 6, r.status);

    check(sqlite3_step(stmt.ptr), SQLITE_DONE);

    return static_cast<int>(sqlite3_last_insert_rowid(db));
  }

  // Used to test duplicate ID handling.
  void addRecordWithId(const Record& r) {
    const char* sql = R"SQL(
            INSERT INTO records
            (id, title, notes, filepath, created_at,
             protection_method, status)
            VALUES (?, ?, ?, ?, ?, ?, ?);
        )SQL";

    Statement stmt;
    prepare(stmt, sql);

    check(sqlite3_bind_int(stmt.ptr, 1, r.id), SQLITE_OK);
    bindText(stmt.ptr, 2, r.title);
    bindText(stmt.ptr, 3, r.notes);
    bindText(stmt.ptr, 4, r.filepath);
    bindText(stmt.ptr, 5, r.created_at);
    bindText(stmt.ptr, 6, r.protection_method);
    bindText(stmt.ptr, 7, r.status);

    check(sqlite3_step(stmt.ptr), SQLITE_DONE);
  }

  // READ: Find a record by ID.
  optional<Record> findRecord(int id) {
    const char* sql = R"SQL(
            SELECT id, title, notes, filepath,
                   created_at, protection_method, status
            FROM records
            WHERE id = ?;
        )SQL";

    Statement stmt;
    prepare(stmt, sql);

    check(sqlite3_bind_int(stmt.ptr, 1, id), SQLITE_OK);

    int rc = sqlite3_step(stmt.ptr);

    if (rc == SQLITE_ROW) {
      return readRecord(stmt.ptr);
    }

    check(rc, SQLITE_DONE);
    return nullopt;
  }

  // READ: List all records.
  vector<Record> getAllRecords() {
    const char* sql = R"SQL(
            SELECT id, title, notes, filepath,
                   created_at, protection_method, status
            FROM records
            ORDER BY id;
        )SQL";

    Statement stmt;
    prepare(stmt, sql);

    vector<Record> records;
    int rc;

    while ((rc = sqlite3_step(stmt.ptr)) == SQLITE_ROW) {
      records.push_back(readRecord(stmt.ptr));
    }

    check(rc, SQLITE_DONE);
    return records;
  }

  // UPDATE: Change title and notes only.
  bool updateRecord(int id, const string& title, const string& notes) {
    const char* sql = R"SQL(
            UPDATE records
            SET title = ?, notes = ?
            WHERE id = ?;
        )SQL";

    Statement stmt;
    prepare(stmt, sql);

    bindText(stmt.ptr, 1, title);
    bindText(stmt.ptr, 2, notes);
    check(sqlite3_bind_int(stmt.ptr, 3, id), SQLITE_OK);

    check(sqlite3_step(stmt.ptr), SQLITE_DONE);
    return sqlite3_changes(db) > 0;
  }

  // DELETE: Removes database row, not the real file.
  bool deleteRecord(int id) {
    const char* sql = "DELETE FROM records WHERE id = ?;";

    Statement stmt;
    prepare(stmt, sql);

    check(sqlite3_bind_int(stmt.ptr, 1, id), SQLITE_OK);

    check(sqlite3_step(stmt.ptr), SQLITE_DONE);
    return sqlite3_changes(db) > 0;
  }
};

int main() {
  // A separate database for CRUD testing.
  const string dbPath = "gx_crud_test.db";

  try {
    int firstId = 0;
    int secondId = 0;

    // First session: create, read, update, delete.
    {
      GXDatabase db(dbPath);
      db.initialize();

      Record first;
      first.title = "GX Test Document";
      first.notes = "Test notes";
      first.filepath = "C:/GX/test.txt";
      first.created_at = "2026-11-21T07:00:00Z";
      first.protection_method = "none";
      first.status = "active";

      firstId = db.addRecord(first);
      cout << "INSERT OK: ID = " << firstId << "\n";

      Record second = first;
      second.title = "Second Document";
      secondId = db.addRecord(second);

      cout << "SECOND INSERT OK: ID = " << secondId << "\n";

      auto found = db.findRecord(firstId);
      if (found) {
        cout << "SELECT OK: " << found->title << "\n";
      }

      bool updated =
          db.updateRecord(firstId, "Updated Document", "Updated notes");

      cout << "UPDATE: " << (updated ? "OK" : "NOT FOUND") << "\n";

      bool deleted = db.deleteRecord(secondId);
      cout << "DELETE: " << (deleted ? "OK" : "NOT FOUND") << "\n";

      auto missing = db.findRecord(-1);
      cout << "MISSING RECORD: " << (missing ? "FOUND" : "NOT FOUND") << "\n";

      // Deliberately try a duplicate ID.
      Record duplicate = first;
      duplicate.id = firstId;

      try {
        db.addRecordWithId(duplicate);
        cout << "DUPLICATE TEST FAILED\n";
      } catch (const exception& e) {
        cout << "DUPLICATE ID REJECTED: " << e.what() << "\n";
      }
    }

    // Second session: reopen database and check persistence.
    {
      GXDatabase db(dbPath);
      db.initialize();

      auto saved = db.findRecord(firstId);

      if (saved && saved->title == "Updated Document") {
        cout << "PERSISTENCE TEST PASSED\n";
      } else {
        cout << "PERSISTENCE TEST FAILED\n";
        return 1;
      }

      cout << "TOTAL RECORDS: " << db.getAllRecords().size() << "\n";
    }

    cout << "GX SQLite CRUD tests completed.\n";
    return 0;

  } catch (const exception& e) {
    cerr << "DATABASE ERROR: " << e.what() << "\n";
    return 1;
  }
}
