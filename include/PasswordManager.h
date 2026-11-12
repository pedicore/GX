#pragma once
#include <string>
#include <iostream>
#include <windows.h>


class Passwordinfo{
private:
std::string  password; 
std::string  Notes; //
std::string  title ; //
std::string  pass_id; //
std::string  filepath; // 
std::string  date_of_creation; //
 


std::string generate_id();
public:

Passwordinfo(
    std::string const &p 
    ,std::string const &n 
    ,std::string const &file_p 
    ,std::string const &t);

 

//get & set
std::string get_date()const; 
std::string get_id()const;
std::string get_title()const;
std::string get_notes()const;
std::string get_filepath() const;
std::string get_filename() const;
void set_title(const  std::string &set_title);
void set_notes(const std::string &set_notes);
//other functions
bool file_exists() const;
bool is_file() const;
bool is_folder() const;
bool file_exists();
};