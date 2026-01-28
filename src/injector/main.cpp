#include <iostream>
#include <string>
#include <Windows.h>
#include "Injector.h"

void printUsage() {
    std::cout << "Minecraft Injection Client - Injector" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  Injector.exe <process_name> <dll_path>" << std::endl;
    std::cout << "  Injector.exe <pid> <dll_path>" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  Injector.exe javaw.exe MinecraftClient.dll" << std::endl;
    std::cout << "  Injector.exe 1234 C:\\path\\to\\MinecraftClient.dll" << std::endl;
    std::cout << std::endl;
    std::cout << "Note: Must run as administrator" << std::endl;
}

std::string getCurrentDirectory() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}

int main(int argc, char* argv[]) {
    std::cout << "=== Minecraft Injection Client ===" << std::endl;
    std::cout << std::endl;
    
    if (argc < 3) {
        printUsage();
        return 1;
    }
    
    std::string target = argv[1];
    std::string dllPath = argv[2];
    
    // If dll path is relative, make it absolute
    if (dllPath.find(':') == std::string::npos) {
        dllPath = getCurrentDirectory() + "\\" + dllPath;
    }
    
    // Check if DLL exists
    if (GetFileAttributesA(dllPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        std::cerr << "Error: DLL not found: " << dllPath << std::endl;
        return 1;
    }
    
    std::cout << "Target: " << target << std::endl;
    std::cout << "DLL: " << dllPath << std::endl;
    std::cout << std::endl;
    
    bool success = false;
    
    // Check if target is a number (PID) or a process name
    if (isdigit(target[0])) {
        DWORD pid = std::stoul(target);
        std::cout << "Injecting into process ID: " << pid << std::endl;
        success = Injector::injectDLL(pid, dllPath);
    } else {
        std::cout << "Injecting into process: " << target << std::endl;
        success = Injector::injectDLL(target, dllPath);
    }
    
    if (success) {
        std::cout << std::endl;
        std::cout << "SUCCESS: DLL injected successfully!" << std::endl;
        std::cout << std::endl;
        std::cout << "Module Controls:" << std::endl;
        std::cout << "  F - Toggle Flight module" << std::endl;
        std::cout << "  E - Toggle ESP module" << std::endl;
        std::cout << "  F1 - List all modules and their status" << std::endl;
        std::cout << "  END - Shutdown client" << std::endl;
        return 0;
    } else {
        std::cerr << std::endl;
        std::cerr << "FAILED: Injection failed" << std::endl;
        std::cerr << "Make sure:" << std::endl;
        std::cerr << "  1. You're running as administrator" << std::endl;
        std::cerr << "  2. The target process is running" << std::endl;
        std::cerr << "  3. The DLL path is correct" << std::endl;
        return 1;
    }
}
