#include "MappingLoader.h"
#include "../utils/Logger.h"
#include <fstream>
#include <sstream>
#include <algorithm>

MappingLoader& MappingLoader::getInstance() {
    static MappingLoader instance;
    return instance;
}

MappingLoader::MappingLoader() 
    : initialized(false) {
}

MappingLoader::~MappingLoader() {
}

bool MappingLoader::initialize(const std::string& mappingsDir) {
    if (initialized) {
        return true;
    }
    
    LOG_INFO("Initializing MCP mapping loader...");
    
    // Try to determine the base directory for mappings
    std::string baseDir = mappingsDir;
    if (baseDir.empty()) {
        // Default to src/mappings relative to current directory
        baseDir = "src/mappings";
    }
    
    std::string fieldsPath = baseDir + "/fields.csv";
    std::string methodsPath = baseDir + "/methods.csv";
    
    // Load field mappings
    if (!loadFieldMappings(fieldsPath)) {
        LOG_WARNING("Failed to load field mappings from: " + fieldsPath);
        // Continue anyway, as mappings are optional fallback
    } else {
        LOG_INFO("Loaded " + std::to_string(fieldMappings.size()) + " field mappings");
    }
    
    // Load method mappings
    if (!loadMethodMappings(methodsPath)) {
        LOG_WARNING("Failed to load method mappings from: " + methodsPath);
        // Continue anyway, as mappings are optional fallback
    } else {
        LOG_INFO("Loaded " + std::to_string(methodMappings.size()) + " method mappings");
    }
    
    initialized = true;
    LOG_INFO("MCP mapping loader initialized");
    return true;
}

bool MappingLoader::loadFieldMappings(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    bool firstLine = true;
    
    while (std::getline(file, line)) {
        // Skip header line
        if (firstLine) {
            firstLine = false;
            continue;
        }
        
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        // Parse CSV: searge,name,side,desc
        std::stringstream ss(line);
        std::string searge, name, side, desc;
        
        if (std::getline(ss, searge, ',') && std::getline(ss, name, ',')) {
            // Clean up any whitespace
            searge.erase(std::remove_if(searge.begin(), searge.end(), ::isspace), searge.end());
            name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());
            
            if (!searge.empty() && !name.empty()) {
                fieldMappings[searge] = name;
            }
        }
    }
    
    file.close();
    return true;
}

bool MappingLoader::loadMethodMappings(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    bool firstLine = true;
    
    while (std::getline(file, line)) {
        // Skip header line
        if (firstLine) {
            firstLine = false;
            continue;
        }
        
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        // Parse CSV: searge,name,side,desc
        std::stringstream ss(line);
        std::string searge, name, side, desc;
        
        if (std::getline(ss, searge, ',') && std::getline(ss, name, ',')) {
            // Clean up any whitespace
            searge.erase(std::remove_if(searge.begin(), searge.end(), ::isspace), searge.end());
            name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());
            
            if (!searge.empty() && !name.empty()) {
                methodMappings[searge] = name;
            }
        }
    }
    
    file.close();
    return true;
}

std::string MappingLoader::getFieldName(const std::string& seargeName) const {
    auto it = fieldMappings.find(seargeName);
    if (it != fieldMappings.end()) {
        return it->second;
    }
    return ""; // Return empty string if not found
}

std::string MappingLoader::getMethodName(const std::string& seargeName) const {
    auto it = methodMappings.find(seargeName);
    if (it != methodMappings.end()) {
        return it->second;
    }
    return ""; // Return empty string if not found
}

bool MappingLoader::hasFieldMapping(const std::string& seargeName) const {
    return fieldMappings.find(seargeName) != fieldMappings.end();
}

bool MappingLoader::hasMethodMapping(const std::string& seargeName) const {
    return methodMappings.find(seargeName) != methodMappings.end();
}

size_t MappingLoader::getFieldMappingCount() const {
    return fieldMappings.size();
}

size_t MappingLoader::getMethodMappingCount() const {
    return methodMappings.size();
}
