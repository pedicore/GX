#pragma once
#include <string>
#include <iostream>
#include <windows.h>


class PassWordinfo{
private:
std::string  password; //
std::string  Notes;//
std::string  title ; 
std::string  pass_id; 
std::string  filepath; //
std::string  date_of_creation; 
std::string  protectionMethod; 


std::string generate_id();
public:

PassWordinfo(
    std::string const &p 
    ,std::string const &n 
    ,std::string const &file_p 
    ,std::string const &t);

 

//get & set
std::string get_date()const; 
std::string get_id()const;
std::string get_title()const;
void set_title(const std::string &set_title);
 
//other functions



};