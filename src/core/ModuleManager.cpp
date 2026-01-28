#include "ModuleManager.h"
#include "../utils/Logger.h"

ModuleManager& ModuleManager::getInstance() {
    static ModuleManager instance;
    return instance;
}

void ModuleManager::addModule(std::shared_ptr<Module> module) {
    if (module) {
        modules.push_back(module);
        LOG_INFO("Module registered: " + module->getName());
    }
}

std::shared_ptr<Module> ModuleManager::getModule(const std::string& name) {
    for (auto& module : modules) {
        if (module->getName() == name) {
            return module;
        }
    }
    return nullptr;
}

void ModuleManager::enableAll() {
    for (auto& module : modules) {
        module->enable();
    }
}

void ModuleManager::disableAll() {
    for (auto& module : modules) {
        module->disable();
    }
}

void ModuleManager::initialize() {
    LOG_INFO("Initializing ModuleManager with " + std::to_string(modules.size()) + " modules");
}
