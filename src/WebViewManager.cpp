#include "WebViewManager.h"
#include <string>
#include <windows.h>
bool WebViewManager:: loadLibrary (std::wstring lib_load){

    if (lib_handler !=nullptr )
    {
        return true;
    }


lib_handler = LoadLibraryW(lib_load.c_str());
    
    
    if (lib_handler == nullptr)
    {
        return false;
    }
    

    return true;
    
    
}


WebViewManager::~WebViewManager(){

    if (lib_handler != nullptr)
    {
        FreeLibrary(lib_handler);
        
    }
    
    lib_handler = nullptr; 



}

bool  WebViewManager::resolveFunctions(){

    if(lib_handler ==nullptr){
        return false; 
        
    }
    FARPROC address;
    address = GetProcAddress(lib_handler , "CreateCoreWebView2EnvironmentWithOptions" );
    func_address = reinterpret_cast<CreateEnvironmentFn>(address);
    
    if (func_address == nullptr)
    {
        return false;
    }
    if (address == nullptr)
    {
        return false;
    }
    
    return true; 

}