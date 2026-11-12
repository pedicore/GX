#include "PasswordManager.h"

#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <system_error>


Passwordinfo::Passwordinfo
    (
    std::string const &p 
    , std::string const &n 
    , std::string const &file_p 
    , std::string const &t 
    )
:    password(p) 
    , Notes(n) 
    , filepath(file_p)
    ,title(t)
{
pass_id = generate_id();

auto now_time =std::chrono::system_clock::now();
std::time_t raw_time =std::chrono::system_clock::to_time_t(now_time);
    
    std::tm local_time{};

if (localtime_s(&local_time, &raw_time) != 0)
{
    date_of_creation = "Unknown";
}
else
{
    std::ostringstream dateStream;
    dateStream << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S");
    date_of_creation = dateStream.str();
}
}
//getters
std::string Passwordinfo::get_date()const{
    return date_of_creation;
}
std::string Passwordinfo::get_title()const{
    return title;
}
std::string Passwordinfo::get_id()const{
    return pass_id;
}
std::string Passwordinfo::get_notes()const{
    return Notes; 
}
std::string Passwordinfo::get_filename() const{
    std::filesystem::path pathobj (filepath); 
    std::string file_name = pathobj.filename().string();
    
    return file_name; 
}

//setters
std::string Passwordinfo::get_filepath() const{
    return filepath;
}
void Passwordinfo::set_title(const std::string &set_title){   
    this->title= set_title;
}
void Passwordinfo::set_notes(const std::string &set_notes){
    this->Notes = set_notes; 
}

//other - functions
std::string Passwordinfo::generate_id(){
    std::string id_alphabet_pool =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string id_generated;
    for (int i = 0;  i<12; i++)
    {   
       int n = std::rand() % 62;       
       id_generated += id_alphabet_pool[n];   
    }
    return id_generated; 
}
bool Passwordinfo::file_exists() const
{
    std::error_code ec;
    bool result = std::filesystem::exists(filepath , ec);
    if (ec)
    {
        return false; 
    }
    return result; 
}
bool Passwordinfo::is_file() const
{
    std::error_code ec;
    bool result = std::filesystem::is_regular_file(filepath, ec);
    if (ec)
    {
        return false;
    }
    return result;
}
bool Passwordinfo::is_folder() const
{
    std::error_code ec;
    bool result = std::filesystem::is_directory(filepath, ec);
    if (ec)
    {
        return false;
    }
    return result;
}






