#pragma once
#include <vector>
#include "PasswordManager.h"

class GXRecordManager
{
private:


std::vector <PasswordManager> pass_records;
public:
    
void find_record(const std::string &rec)const; 
void add_record(const PasswordManager &records);
std::size_t getRecordCount()const;
bool  remove_record(const std::string &id);
bool update_record(const std::string &id , const std::string &new_title);

    








    
};


