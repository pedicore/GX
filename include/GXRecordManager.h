#pragma once
#include <vector>
#include "PasswordManager.h"

class GXRecordManager
{
private:


std::vector <Passwordinfo> pass_records;
public:
    
void find_record(const std::string &rec)const; 
void add_record(const Passwordinfo &records);
std::size_t getRecordCount()const;
bool  remove_record(const std::string &id);
   
    








    
};


