#include "JNIUtils.h"
#include "../utils/Logger.h"
#include <Windows.h>
#include <TlHelp32.h>

// JNI function pointer types
typedef jint (JNICALL *JNI_GetCreatedJavaVMs_t)(JavaVM**, jsize, jsize*);

JNIUtils& JNIUtils::getInstance() {
    static JNIUtils instance;
    return instance;
}

JNIUtils::JNIUtils() : jvm(nullptr), env(nullptr), attached(false) {
}

JNIUtils::~JNIUtils() {
    detachFromJVM();
}

JavaVM* JNIUtils::findJVM() {
    // Try to load the JVM DLL
    HMODULE jvmDll = GetModuleHandleA("jvm.dll");
    
    if (!jvmDll) {
        LOG_ERROR("Failed to find jvm.dll");
        return nullptr;
    }
    
    // Get the JNI_GetCreatedJavaVMs function
    JNI_GetCreatedJavaVMs_t JNI_GetCreatedJavaVMs = 
        (JNI_GetCreatedJavaVMs_t)GetProcAddress(jvmDll, "JNI_GetCreatedJavaVMs");
    
    if (!JNI_GetCreatedJavaVMs) {
        LOG_ERROR("Failed to find JNI_GetCreatedJavaVMs");
        return nullptr;
    }
    
    // Get created JVMs
    JavaVM* jvms[1];
    jsize nVMs;
    
    jint result = JNI_GetCreatedJavaVMs(jvms, 1, &nVMs);
    
    if (result != JNI_OK || nVMs == 0) {
        LOG_ERROR("No JVM found in process");
        return nullptr;
    }
    
    return jvms[0];
}

bool JNIUtils::attachToJVM() {
    if (attached) {
        LOG_WARNING("Already attached to JVM");
        return true;
    }
    
    LOG_INFO("Attempting to attach to JVM...");
    
    jvm = findJVM();
    if (!jvm) {
        LOG_ERROR("Failed to find JVM");
        return false;
    }
    
    jint result = jvm->AttachCurrentThread((void**)&env, nullptr);
    
    if (result != JNI_OK) {
        LOG_ERROR("Failed to attach to JVM");
        return false;
    }
    
    attached = true;
    LOG_INFO("Successfully attached to JVM");
    return true;
}

void JNIUtils::detachFromJVM() {
    if (attached && jvm) {
        jvm->DetachCurrentThread();
        attached = false;
        LOG_INFO("Detached from JVM");
    }
}

jclass JNIUtils::findClass(const std::string& className) {
    if (!env) {
        LOG_ERROR("JNI environment not available");
        return nullptr;
    }
    
    jclass clazz = env->FindClass(className.c_str());
    
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        LOG_ERROR("Failed to find class: " + className);
        return nullptr;
    }
    
    if (!clazz) {
        LOG_ERROR("Class not found: " + className);
        return nullptr;
    }
    
    // Make it a global reference so it persists
    return (jclass)env->NewGlobalRef(clazz);
}

jmethodID JNIUtils::getMethodID(jclass clazz, const std::string& methodName, const std::string& signature) {
    if (!env || !clazz) return nullptr;
    
    jmethodID method = env->GetMethodID(clazz, methodName.c_str(), signature.c_str());
    
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        LOG_ERROR("Failed to find method: " + methodName);
        return nullptr;
    }
    
    return method;
}

jfieldID JNIUtils::getFieldID(jclass clazz, const std::string& fieldName, const std::string& signature) {
    if (!env || !clazz) return nullptr;
    
    jfieldID field = env->GetFieldID(clazz, fieldName.c_str(), signature.c_str());
    
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        LOG_ERROR("Failed to find field: " + fieldName);
        return nullptr;
    }
    
    return field;
}

jmethodID JNIUtils::getStaticMethodID(jclass clazz, const std::string& methodName, const std::string& signature) {
    if (!env || !clazz) return nullptr;
    
    jmethodID method = env->GetStaticMethodID(clazz, methodName.c_str(), signature.c_str());
    
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        LOG_ERROR("Failed to find static method: " + methodName);
        return nullptr;
    }
    
    return method;
}

jfieldID JNIUtils::getStaticFieldID(jclass clazz, const std::string& fieldName, const std::string& signature) {
    if (!env || !clazz) return nullptr;
    
    jfieldID field = env->GetStaticFieldID(clazz, fieldName.c_str(), signature.c_str());
    
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        LOG_ERROR("Failed to find static field: " + fieldName);
        return nullptr;
    }
    
    return field;
}

jobject JNIUtils::getObjectField(jobject obj, jfieldID fieldID) {
    if (!env || !obj || !fieldID) return nullptr;
    return env->GetObjectField(obj, fieldID);
}

void JNIUtils::setObjectField(jobject obj, jfieldID fieldID, jobject value) {
    if (!env || !obj || !fieldID) return;
    env->SetObjectField(obj, fieldID, value);
}

jboolean JNIUtils::getBooleanField(jobject obj, jfieldID fieldID) {
    if (!env || !obj || !fieldID) return JNI_FALSE;
    return env->GetBooleanField(obj, fieldID);
}

void JNIUtils::setBooleanField(jobject obj, jfieldID fieldID, jboolean value) {
    if (!env || !obj || !fieldID) return;
    env->SetBooleanField(obj, fieldID, value);
}

jfloat JNIUtils::getFloatField(jobject obj, jfieldID fieldID) {
    if (!env || !obj || !fieldID) return 0.0f;
    return env->GetFloatField(obj, fieldID);
}

void JNIUtils::setFloatField(jobject obj, jfieldID fieldID, jfloat value) {
    if (!env || !obj || !fieldID) return;
    env->SetFloatField(obj, fieldID, value);
}

jdouble JNIUtils::getDoubleField(jobject obj, jfieldID fieldID) {
    if (!env || !obj || !fieldID) return 0.0;
    return env->GetDoubleField(obj, fieldID);
}

void JNIUtils::setDoubleField(jobject obj, jfieldID fieldID, jdouble value) {
    if (!env || !obj || !fieldID) return;
    env->SetDoubleField(obj, fieldID, value);
}

void JNIUtils::callVoidMethod(jobject obj, jmethodID methodID, ...) {
    if (!env || !obj || !methodID) return;
    
    va_list args;
    va_start(args, methodID);
    env->CallVoidMethodV(obj, methodID, args);
    va_end(args);
}

jobject JNIUtils::callObjectMethod(jobject obj, jmethodID methodID, ...) {
    if (!env || !obj || !methodID) return nullptr;
    
    va_list args;
    va_start(args, methodID);
    jobject result = env->CallObjectMethodV(obj, methodID, args);
    va_end(args);
    
    return result;
}

jfieldID JNIUtils::findFieldBySignature(jclass clazz, const std::string& signature) {
    if (!env || !clazz) return nullptr;
    
    // Get the Class object methods
    jclass classClass = env->FindClass("java/lang/Class");
    if (!classClass) {
        if (env->ExceptionCheck()) env->ExceptionClear();
        return nullptr;
    }
    
    jmethodID getDeclaredFieldsMethod = env->GetMethodID(classClass, "getDeclaredFields", "()[Ljava/lang/reflect/Field;");
    if (!getDeclaredFieldsMethod) {
        if (env->ExceptionCheck()) env->ExceptionClear();
        env->DeleteLocalRef(classClass);
        return nullptr;
    }
    
    // Get all fields
    jobjectArray fields = (jobjectArray)env->CallObjectMethod(clazz, getDeclaredFieldsMethod);
    if (!fields || env->ExceptionCheck()) {
        if (env->ExceptionCheck()) env->ExceptionClear();
        env->DeleteLocalRef(classClass);
        return nullptr;
    }
    
    jint fieldCount = env->GetArrayLength(fields);
    
    // Get Field class methods
    jclass fieldClass = env->FindClass("java/lang/reflect/Field");
    jmethodID getNameMethod = env->GetMethodID(fieldClass, "getName", "()Ljava/lang/String;");
    jmethodID getTypeMethod = env->GetMethodID(fieldClass, "getType", "()Ljava/lang/Class;");
    
    if (!getNameMethod || !getTypeMethod) {
        if (env->ExceptionCheck()) env->ExceptionClear();
        env->DeleteLocalRef(fields);
        env->DeleteLocalRef(fieldClass);
        env->DeleteLocalRef(classClass);
        return nullptr;
    }
    
    jclass typeClass = env->FindClass("java/lang/Class");
    jmethodID getNameTypeMethod = env->GetMethodID(typeClass, "getName", "()Ljava/lang/String;");
    
    jfieldID result = nullptr;
    
    // Iterate through fields
    for (jint i = 0; i < fieldCount; i++) {
        jobject field = env->GetObjectArrayElement(fields, i);
        if (!field) continue;
        
        // Get field type
        jobject fieldType = env->CallObjectMethod(field, getTypeMethod);
        if (!fieldType) {
            if (env->ExceptionCheck()) env->ExceptionClear();
            env->DeleteLocalRef(field);
            continue;
        }
        
        // Get type name
        jstring typeName = (jstring)env->CallObjectMethod(fieldType, getNameTypeMethod);
        if (!typeName) {
            if (env->ExceptionCheck()) env->ExceptionClear();
            env->DeleteLocalRef(fieldType);
            env->DeleteLocalRef(field);
            continue;
        }
        
        const char* typeStr = env->GetStringUTFChars(typeName, nullptr);
        std::string typeString(typeStr);
        env->ReleaseStringUTFChars(typeName, typeStr);
        
        // Convert signature to Java class name format
        std::string targetType = signature;
        // Convert signature like "Lrz;" to "rz"
        if (targetType.size() > 2 && targetType[0] == 'L' && targetType[targetType.size()-1] == ';') {
            targetType = targetType.substr(1, targetType.size() - 2);
            // Replace / with .
            for (size_t j = 0; j < targetType.size(); j++) {
                if (targetType[j] == '/') targetType[j] = '.';
            }
        }
        
        // Check if type matches
        if (typeString == targetType) {
            // Get field name
            jstring fieldName = (jstring)env->CallObjectMethod(field, getNameMethod);
            if (fieldName) {
                const char* nameStr = env->GetStringUTFChars(fieldName, nullptr);
                std::string fieldNameStr(nameStr);
                env->ReleaseStringUTFChars(fieldName, nameStr);
                
                // Get the field ID
                result = env->GetFieldID(clazz, fieldNameStr.c_str(), signature.c_str());
                if (env->ExceptionCheck()) {
                    env->ExceptionClear();
                    result = nullptr;
                }
                
                env->DeleteLocalRef(fieldName);
            }
            env->DeleteLocalRef(typeName);
            env->DeleteLocalRef(fieldType);
            env->DeleteLocalRef(field);
            break;
        }
        
        env->DeleteLocalRef(typeName);
        env->DeleteLocalRef(fieldType);
        env->DeleteLocalRef(field);
    }
    
    env->DeleteLocalRef(typeClass);
    env->DeleteLocalRef(fieldClass);
    env->DeleteLocalRef(fields);
    env->DeleteLocalRef(classClass);
    
    return result;
}

jfieldID JNIUtils::findBooleanField(jclass clazz, int index) {
    if (!env || !clazz) return nullptr;
    
    // Get the Class object methods
    jclass classClass = env->FindClass("java/lang/Class");
    if (!classClass) {
        if (env->ExceptionCheck()) env->ExceptionClear();
        return nullptr;
    }
    
    jmethodID getDeclaredFieldsMethod = env->GetMethodID(classClass, "getDeclaredFields", "()[Ljava/lang/reflect/Field;");
    if (!getDeclaredFieldsMethod) {
        if (env->ExceptionCheck()) env->ExceptionClear();
        env->DeleteLocalRef(classClass);
        return nullptr;
    }
    
    // Get all fields
    jobjectArray fields = (jobjectArray)env->CallObjectMethod(clazz, getDeclaredFieldsMethod);
    if (!fields || env->ExceptionCheck()) {
        if (env->ExceptionCheck()) env->ExceptionClear();
        env->DeleteLocalRef(classClass);
        return nullptr;
    }
    
    jint fieldCount = env->GetArrayLength(fields);
    
    // Get Field class methods
    jclass fieldClass = env->FindClass("java/lang/reflect/Field");
    jmethodID getNameMethod = env->GetMethodID(fieldClass, "getName", "()Ljava/lang/String;");
    jmethodID getTypeMethod = env->GetMethodID(fieldClass, "getType", "()Ljava/lang/Class;");
    
    if (!getNameMethod || !getTypeMethod) {
        if (env->ExceptionCheck()) env->ExceptionClear();
        env->DeleteLocalRef(fields);
        env->DeleteLocalRef(fieldClass);
        env->DeleteLocalRef(classClass);
        return nullptr;
    }
    
    // Get the boolean primitive type using Boolean.TYPE
    jclass boolClass = env->FindClass("java/lang/Boolean");
    jfieldID typeFieldID = env->GetStaticFieldID(boolClass, "TYPE", "Ljava/lang/Class;");
    jobject boolType = nullptr;
    if (typeFieldID) {
        boolType = env->GetStaticObjectField(boolClass, typeFieldID);
    }
    if (!boolType) {
        // Fallback: just check if field signature is "Z"
        if (env->ExceptionCheck()) env->ExceptionClear();
        env->DeleteLocalRef(boolClass);
        env->DeleteLocalRef(fieldClass);
        env->DeleteLocalRef(fields);
        env->DeleteLocalRef(classClass);
        return nullptr;
    }
    
    jfieldID result = nullptr;
    int currentIndex = 0;
    
    // Iterate through fields looking for boolean fields
    for (jint i = 0; i < fieldCount; i++) {
        jobject field = env->GetObjectArrayElement(fields, i);
        if (!field) continue;
        
        // Get field type
        jobject fieldType = env->CallObjectMethod(field, getTypeMethod);
        if (!fieldType) {
            if (env->ExceptionCheck()) env->ExceptionClear();
            env->DeleteLocalRef(field);
            continue;
        }
        
        // Check if it's a boolean
        if (env->IsSameObject(fieldType, boolType)) {
            if (currentIndex == index) {
                // Get field name
                jstring fieldName = (jstring)env->CallObjectMethod(field, getNameMethod);
                if (fieldName) {
                    const char* nameStr = env->GetStringUTFChars(fieldName, nullptr);
                    std::string fieldNameStr(nameStr);
                    env->ReleaseStringUTFChars(fieldName, nameStr);
                    
                    // Get the field ID
                    result = env->GetFieldID(clazz, fieldNameStr.c_str(), "Z");
                    if (env->ExceptionCheck()) {
                        env->ExceptionClear();
                        result = nullptr;
                    }
                    
                    env->DeleteLocalRef(fieldName);
                }
                env->DeleteLocalRef(fieldType);
                env->DeleteLocalRef(field);
                break;
            }
            currentIndex++;
        }
        
        env->DeleteLocalRef(fieldType);
        env->DeleteLocalRef(field);
    }
    
    env->DeleteLocalRef(boolType);
    env->DeleteLocalRef(boolClass);
    env->DeleteLocalRef(fieldClass);
    env->DeleteLocalRef(fields);
    env->DeleteLocalRef(classClass);
    
    return result;
}
