#pragma once
#include <Windows.h>
#include <string>

class Injector {
public:
    // Inject DLL into a process by name
    static bool injectDLL(const std::string& processName, const std::string& dllPath);
    
    // Inject DLL into a process by PID
    static bool injectDLL(DWORD processId, const std::string& dllPath);
    
private:
    // Find process ID by name
    static DWORD getProcessIdByName(const std::string& processName);
    
    // Perform the actual injection
    static bool performInjection(HANDLE hProcess, const std::string& dllPath);
};
