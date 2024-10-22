#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <sqlite3.h>

class Database 
{
public:

    Database(const std::string& dbFile);

    ~Database();

    bool verifyUser(int id);

    bool verifyUserByNameAndPassword(const std::string& username, const std::string& password);

private:
    sqlite3 *db; 
};

#endif 
