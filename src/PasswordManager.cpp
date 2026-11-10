#include "PasswordManager.h"
#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>



PassWordinfo::PassWordinfo(
    std::string const &p 
    , std::string const &n 
    , std::string const &file_p 
    , std::string const &t 
    )
:password(p) 
, Notes(n) 
, filepath(file_p)
,title(t)
{

pass_id = generate_id();

auto now_time =std::chrono::system_clock::now();
std::time_t raw_time =std::chrono::system_clock::to_time_t(now_time);
    
    std::tm local_time{};
    localtime_s(&local_time, &raw_time);
    std::ostringstream dateStream;
    dateStream << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S");
    date_of_creation = dateStream.str();
}
//getters
std::string PassWordinfo::get_date()const{
    return date_of_creation;
}
std::string PassWordinfo::get_title()const{
    return title;
}
std::string PassWordinfo::get_id()const{
    return pass_id;
}
//setters
void PassWordinfo::set_title(const std::string &set_title){   
    this->title= set_title;
}

//other - functions

std::string PassWordinfo::generate_id(){

    std::string id_alphabet_pool = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    
    std::string id_generated ;
    for (int i = 0;  i<12; i++)
    {   
       int n = std::rand() % 62;
        
       id_generated += id_alphabet_pool[n];
        
    }
    return id_generated; 
}







