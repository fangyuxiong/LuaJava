//
// Created by Xiong.Fangyu 2019/03/13.
//

#ifndef GLOBAL_DEFINE_H
#define GLOBAL_DEFINE_H

#include <jni.h>
#include <string.h>
#include "lua.h"
#include "utils.h"
#include "luaconf.h"

#define ERROR_FUN "__JAPI_ERROR_FUN"
#define OBJECT_CLASS "Ljava/lang/Object;"
#define STRING_CLASS "Ljava/lang/String;"
#define THROWABLE_CLASS "Ljava/lang/Throwable;"
#define JAVA_PATH "com/xfy/luajava/"
#define LUAVALUE_CLASS "L" JAVA_PATH "LuaValue;"

#define GLOBAL_KEY "LUA_REGISTRYINDEX"
#define isGlobal(k) (strstr(k, GLOBAL_KEY) != NULL)

#define METATABLE_PREFIX "__M_"
#define METATABLE_FORMAT METATABLE_PREFIX "%s"
#define isJavaUserdata(ud) ((ud) && (ud->jobj) && (strstr(ud->name, METATABLE_PREFIX)))

#define DEFAULT_SIG "([" LUAVALUE_CLASS ")[" LUAVALUE_CLASS

#define GetString(env, js) (!js ? NULL : (*env)->GetStringUTFChars(env, js, 0))
#define ReleaseChar(env, js, c) ((js && c) ? (*env)->ReleaseStringUTFChars(env, js, c) : (void *)c)

#define FREE(env, obj) if ((obj) && (*env)->GetObjectRefType(env, obj) == JNILocalRefType) (*env)->DeleteLocalRef(env, obj);
#if defined(J_API_INFO)
#define ExceptionDescribe(env) (*env)->ExceptionDescribe(env);
jobject _global(JNIEnv *, jobject);
void _unglobal(JNIEnv *, jobject);
#define GLOBAL(env, obj) _global(env, obj)
#define UNGLOBAL(env, obj) _unglobal(env, obj)
#else
#define ExceptionDescribe(env)
#define GLOBAL(env, obj) (*env)->NewGlobalRef(env, obj)
#define UNGLOBAL(env, obj) (*env)->DeleteGlobalRef(env, obj)
#endif

typedef JNIEnv **UDJNIEnv;
typedef jobject *UDjobject;
typedef jclass *UDjclass;
typedef jmethodID *UDjmethod;
typedef jobjectArray *UDjarray;
#define getuserdata(ud) (*ud)

struct javaUserdata
{
    jobject jobj;
    int isStrongRef;
    const char *name;
    int isSetKey;
};
typedef struct javaUserdata javaUserdata;
typedef javaUserdata *UDjavaobject;

/**
 * 将src table中的数据拷贝到desc table中
 */
void copyTable(lua_State *L, int src, int desc);
#endif // GLOBAL_DEFINE_H