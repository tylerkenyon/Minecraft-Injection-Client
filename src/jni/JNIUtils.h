#pragma once
#include <jni.h>
#include <string>
#include <memory>

class JNIUtils {
public:
    static JNIUtils& getInstance();
    
    bool attachToJVM();
    void detachFromJVM();
    
    JNIEnv* getEnv() const { return env; }
    JavaVM* getJVM() const { return jvm; }
    
    bool isAttached() const { return attached; }
    
    // Helper functions for JNI operations
    jclass findClass(const std::string& className);
    jmethodID getMethodID(jclass clazz, const std::string& methodName, const std::string& signature);
    jfieldID getFieldID(jclass clazz, const std::string& fieldName, const std::string& signature);
    jmethodID getStaticMethodID(jclass clazz, const std::string& methodName, const std::string& signature);
    jfieldID getStaticFieldID(jclass clazz, const std::string& fieldName, const std::string& signature);
    
    // Object field access
    jobject getObjectField(jobject obj, jfieldID fieldID);
    void setObjectField(jobject obj, jfieldID fieldID, jobject value);
    
    // Boolean field access
    jboolean getBooleanField(jobject obj, jfieldID fieldID);
    void setBooleanField(jobject obj, jfieldID fieldID, jboolean value);
    
    // Float field access
    jfloat getFloatField(jobject obj, jfieldID fieldID);
    void setFloatField(jobject obj, jfieldID fieldID, jfloat value);
    
    // Double field access
    jdouble getDoubleField(jobject obj, jfieldID fieldID);
    void setDoubleField(jobject obj, jfieldID fieldID, jdouble value);
    
    // Call methods
    void callVoidMethod(jobject obj, jmethodID methodID, ...);
    jobject callObjectMethod(jobject obj, jmethodID methodID, ...);

private:
    JNIUtils();
    ~JNIUtils();
    
    JavaVM* jvm;
    JNIEnv* env;
    bool attached;
    
    JavaVM* findJVM();
};
