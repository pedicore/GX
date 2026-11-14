#include "GXRecordManager.h"
#include "PasswordManager.h"
#include <iostream> 
#include <vector>



void GXRecordManager::add_record(const PasswordManager &records){
pass_records.push_back(records);
}


//getters
std::size_t GXRecordManager::getRecordCount()const{
return pass_records.size();
}
//other_functions
void GXRecordManager::find_record(const std::string &rec)const{
    for(const auto& record :pass_records){
            if (record.get_id() == rec)
            {
            std::cout << "Record found: "
                      << record.get_title() << '\n';
            return;
            }
            
    }
    std::cout << "Record not found\n";
}

bool GXRecordManager::remove_record(const std::string& id)
{
    for (auto it = pass_records.begin(); it != pass_records.end(); ++it)
    {
        if (it->get_id() == id)
        {
            pass_records.erase(it);
            return true;
        }
    }
    return false;
}
bool GXRecordManager::update_record(const std::string &id , const std::string &new_title){
    for (auto it = pass_records.begin(); it != pass_records.end(); ++it){
        if (it->get_id() == id)
        {
            it->set_title(new_title);

            return true ; 
        }
    }
    return false; 
}
