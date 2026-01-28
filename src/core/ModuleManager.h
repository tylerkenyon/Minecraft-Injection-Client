#pragma once
#include <vector>
#include <memory>
#include "Module.h"

class ModuleManager {
public:
    static ModuleManager& getInstance();
    
    // Add a module to the manager
    void addModule(std::shared_ptr<Module> module);
    
    // Get all modules
    const std::vector<std::shared_ptr<Module>>& getModules() const { return modules; }
    
    // Get a module by name
    std::shared_ptr<Module> getModule(const std::string& name);
    
    // Enable/disable all modules
    void enableAll();
    void disableAll();
    
    // Initialize all modules
    void initialize();

private:
    ModuleManager() = default;
    ~ModuleManager() = default;
    
    std::vector<std::shared_ptr<Module>> modules;
};
