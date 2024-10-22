#include "Database.h"
#include <iostream>
#include <sqlite3.h>

Database::Database(const std::string& dbFile) 
{
    if (sqlite3_open(dbFile.c_str(), &db)) 
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    }
}

Database::~Database() 
{
    if (db) sqlite3_close(db);
}

bool Database::verifyUserByNameAndPassword(const std::string& username, const std::string& password) 
{
    const char* sql = "SELECT COUNT(*) FROM users WHERE username = ? AND password = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) 
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    int count = 0;

    if (rc == SQLITE_ROW) 
    {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    return count > 0;
}



