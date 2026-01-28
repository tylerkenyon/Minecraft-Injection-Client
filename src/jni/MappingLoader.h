#pragma once
#include <string>
#include <unordered_map>
#include <memory>

// MappingLoader - Loads and provides access to MCP mappings
// Reads CSV files from src/mappings directory to translate obfuscated names to clean MCP names
class MappingLoader {
public:
    static MappingLoader& getInstance();
    
    // Initialize and load all mapping files
    bool initialize(const std::string& mappingsDir = "");
    
    // Get deobfuscated field name from searge name (e.g. "field_71439_g" -> "thePlayer")
    std::string getFieldName(const std::string& seargeName) const;
    
    // Get deobfuscated method name from searge name (e.g. "func_71410_x" -> "updateCameraAndRender")
    std::string getMethodName(const std::string& seargeName) const;
    
    // Check if a field/method mapping exists
    bool hasFieldMapping(const std::string& seargeName) const;
    bool hasMethodMapping(const std::string& seargeName) const;
    
    // Get all available mappings (for debugging)
    size_t getFieldMappingCount() const;
    size_t getMethodMappingCount() const;
    
private:
    MappingLoader();
    ~MappingLoader();
    
    bool loadFieldMappings(const std::string& filePath);
    bool loadMethodMappings(const std::string& filePath);
    
    std::unordered_map<std::string, std::string> fieldMappings;
    std::unordered_map<std::string, std::string> methodMappings;
    
    bool initialized;
};
