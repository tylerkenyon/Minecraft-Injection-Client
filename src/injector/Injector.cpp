#include "Injector.h"
#include <TlHelp32.h>
#include <iostream>

bool Injector::enableDebugPrivilege() {
    HANDLE hToken = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        return false;
    }

    TOKEN_PRIVILEGES tp{};
    LUID luid{};
    if (!LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &luid)) {
        CloseHandle(hToken);
        return false;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), nullptr, nullptr)) {
        CloseHandle(hToken);
        return false;
    }

    DWORD adjustError = GetLastError();
    CloseHandle(hToken);
    return adjustError == ERROR_SUCCESS;
}

DWORD Injector::getProcessIdByName(const std::string& processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }
    
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    
    DWORD processId = 0;
    
    if (Process32First(hSnapshot, &pe32)) {
        do {
            if (_stricmp(processName.c_str(), pe32.szExeFile) == 0) {
                processId = pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }
    
    CloseHandle(hSnapshot);
    return processId;
}

bool Injector::performInjection(HANDLE hProcess, const std::string& dllPath) {
    // Allocate memory in target process for DLL path
    SIZE_T pathSize = dllPath.length() + 1;
    LPVOID pRemotePath = VirtualAllocEx(hProcess, nullptr, pathSize, 
                                        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    if (!pRemotePath) {
        std::cerr << "Failed to allocate memory in target process" << std::endl;
        return false;
    }
    
    // Write DLL path to target process
    if (!WriteProcessMemory(hProcess, pRemotePath, dllPath.c_str(), pathSize, nullptr)) {
        std::cerr << "Failed to write DLL path to target process" << std::endl;
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        return false;
    }
    
    // Get address of LoadLibraryA
    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    if (!hKernel32) {
        std::cerr << "Failed to get kernel32.dll handle" << std::endl;
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        return false;
    }
    
    LPVOID pLoadLibrary = (LPVOID)GetProcAddress(hKernel32, "LoadLibraryA");
    if (!pLoadLibrary) {
        std::cerr << "Failed to get LoadLibraryA address" << std::endl;
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        return false;
    }
    
    // Create remote thread to load DLL
    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, 
                                       (LPTHREAD_START_ROUTINE)pLoadLibrary, 
                                       pRemotePath, 0, nullptr);
    
    if (!hThread) {
        std::cerr << "Failed to create remote thread" << std::endl;
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        return false;
    }
    
    // Wait for thread to finish
    WaitForSingleObject(hThread, INFINITE);
    DWORD remoteExitCode = 0;
    if (!GetExitCodeThread(hThread, &remoteExitCode)) {
        std::cerr << "Failed to read remote thread exit code" << std::endl;
        CloseHandle(hThread);
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        return false;
    }
    if (remoteExitCode == 0) {
        std::cerr << "Remote LoadLibrary failed in target process" << std::endl;
        CloseHandle(hThread);
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        return false;
    }
    
    // Cleanup
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
    
    return true;
}

bool Injector::injectDLL(const std::string& processName, const std::string& dllPath) {
    DWORD processId = getProcessIdByName(processName);
    
    if (processId == 0) {
        std::cerr << "Process not found: " << processName << std::endl;
        return false;
    }
    
    return injectDLL(processId, dllPath);
}

bool Injector::injectDLL(DWORD processId, const std::string& dllPath) {
    if (GetFileAttributesA(dllPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        std::cerr << "DLL not found: " << dllPath << std::endl;
        return false;
    }

    if (!enableDebugPrivilege()) {
        std::cerr << "Warning: Failed to enable SeDebugPrivilege" << std::endl;
    }

    // Open target process
    HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | 
                                  PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, 
                                  FALSE, processId);
    
    if (!hProcess) {
        std::cerr << "Failed to open process (PID: " << processId << ")" << std::endl;
        std::cerr << "Make sure you're running as administrator" << std::endl;
        return false;
    }
    
    bool success = performInjection(hProcess, dllPath);
    
    CloseHandle(hProcess);
    return success;
}
