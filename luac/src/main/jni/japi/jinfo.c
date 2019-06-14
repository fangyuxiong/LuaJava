//
// Created by Xiong.Fangyu 2019/03/13.
//

#include "jinfo.h"
#include "lauxlib.h"
#include "mlog.h"
#include "cache.h"
#include "llimits.h"

static jboolean init = 0;
JavaVM * g_jvm;
// ------------global ref
jclass StringClass = NULL;
jclass LuaValue = NULL;
jclass Globals = NULL;
jclass LuaNumber = NULL;
jclass LuaString = NULL;
jclass LuaTable = NULL;
jclass LuaFunction = NULL;
jclass LuaUserdata = NULL;
jclass LuaThread = NULL;
jclass JavaUserdata = NULL;
jclass InvokeError = NULL;
jclass RuntimeException = NULL;
// ------------single instance
jobject Lua_TRUE = NULL;
jobject Lua_FALSE = NULL;
jobject Lua_NIL = NULL;

// ------------globals
jmethodID Globals__onLuaRequire = NULL;
jmethodID Globals__onLuaGC = NULL;
// ------------value
jmethodID LuaValue_type = NULL;
jmethodID LuaValue_nativeGlobalKey = NULL;
// ------------number
jmethodID LuaNumber_I = NULL;
jmethodID LuaNumber_D = NULL;
jfieldID LuaNumber_value = NULL;
// ------------boolean
jfieldID LuaBoolean_value = NULL;
// ------------string
jmethodID LuaString_C = NULL;
jfieldID LuaString_value = NULL;
// ------------table
jmethodID LuaTable_C = NULL;
// ------------function
jmethodID LuaFunction_C = NULL;
// ------------thread
jmethodID LuaThread_C = NULL;
// ------------userdata
jmethodID LuaUserdata_C = NULL;
jfieldID LuaUserdata_luaclassName = NULL;
jfieldID LuaUserdata_nativeGlobalKey = NULL;
// ------------exception
// jmethodID InvokeError_C = NULL;
// jmethodID InvokeError_CT = NULL;

void initJavaInfo(JNIEnv *env)
{
    if (init)
    {
        return;
    }

    StringClass = GLOBAL(env, (*env)->FindClass(env, "java/lang/String"));

    Globals = GLOBAL(env, findTypeClass(env, "Globals"));
    Globals__onLuaRequire = (*env)->GetStaticMethodID(env, Globals, "__onLuaRequire", "(J" STRING_CLASS ")" OBJECT_CLASS);
    Globals__onLuaGC = (*env)->GetStaticMethodID(env, Globals, "__onLuaGC", "(J)V");

    LuaValue = GLOBAL(env, findTypeClass(env, "LuaValue"));
    LuaValue_type = (*env)->GetMethodID(env, LuaValue, "type", "()I");
    LuaValue_nativeGlobalKey = (*env)->GetMethodID(env, LuaValue, "nativeGlobalKey", "()" STRING_CLASS);

    LuaNumber = GLOBAL(env, findTypeClass(env, "LuaNumber"));
    LuaNumber_I = (*env)->GetStaticMethodID(env, LuaNumber, JAVA_VALUE_OF, "(I)L" JAVA_PATH "LuaNumber;");
    LuaNumber_D = findConstructor(env, LuaNumber, "D");
    LuaNumber_value = (*env)->GetFieldID(env, LuaNumber, "value", "D");

    jclass lb = findTypeClass(env, "LuaBoolean");
    LuaBoolean_value = (*env)->GetFieldID(env, lb, "value", "Z");

    LuaString = GLOBAL(env, findTypeClass(env, "LuaString"));
    LuaString_C = findConstructor(env, LuaString, STRING_CLASS);
    LuaString_value = (*env)->GetFieldID(env, LuaString, "value", STRING_CLASS);

    LuaTable = GLOBAL(env, findTypeClass(env, "LuaTable"));
    LuaTable_C = findConstructor(env, LuaTable, "J" STRING_CLASS);

    LuaFunction = GLOBAL(env, findTypeClass(env, "LuaFunction"));
    LuaFunction_C = findConstructor(env, LuaFunction, "J" STRING_CLASS);

    LuaUserdata = GLOBAL(env, findTypeClass(env, "LuaUserdata"));
    LuaUserdata_C = findConstructor(env, LuaUserdata, "J" STRING_CLASS);
    LuaUserdata_luaclassName = (*env)->GetFieldID(env, LuaUserdata, "luaclassName", STRING_CLASS);
    LuaUserdata_nativeGlobalKey = (*env)->GetFieldID(env, LuaUserdata, "nativeGlobalKey", STRING_CLASS);
    JavaUserdata = GLOBAL(env, findTypeClass(env, "JavaUserdata"));

    LuaThread = GLOBAL(env, findTypeClass(env, "LuaThread"));
    LuaThread_C = findConstructor(env, LuaThread, "J" STRING_CLASS);

    jclass LuaBoolean = findTypeClass(env, "LuaBoolean");
    jmethodID LuaBoolean_TRUE = (*env)->GetStaticMethodID(env, LuaBoolean, "TRUE", "()L" JAVA_PATH "LuaBoolean;");
    jmethodID LuaBoolean_FALSE = (*env)->GetStaticMethodID(env, LuaBoolean, "FALSE", "()L" JAVA_PATH "LuaBoolean;");
    Lua_TRUE = GLOBAL(env, (*env)->CallStaticObjectMethod(env, LuaBoolean, LuaBoolean_TRUE));
    Lua_FALSE = GLOBAL(env, (*env)->CallStaticObjectMethod(env, LuaBoolean, LuaBoolean_FALSE));

    jclass LuaNil = findTypeClass(env, "LuaNil");
    jmethodID LuaNil_NIL = (*env)->GetStaticMethodID(env, LuaNil, "NIL", "()L" JAVA_PATH "LuaNil;");
    Lua_NIL = GLOBAL(env, (*env)->CallStaticObjectMethod(env, LuaNil, LuaNil_NIL));

    init = 1;
}

jobject newLuaNumber(JNIEnv *env, jdouble num)
{
    if (num == ((jint)num))
    {
        return (*env)->CallStaticObjectMethod(env, LuaNumber, LuaNumber_I, (jint)num);
    }
    return (*env)->NewObject(env, LuaNumber, LuaNumber_D, num);
}

jobject newLuaString(JNIEnv *env, const char *s)
{
    jstring str = (*env)->NewStringUTF(env, s);
    jobject ret = (*env)->NewObject(env, LuaString, LuaString_C, str);
    FREE(env, str);
    return ret;
}

jobject newLuaTable(JNIEnv *env, lua_State *L, int idx)
{
    lua_lock(L);
    const char * key = copyValueToGNV(L, idx);
    jobject ret;

#if defined(SAVE_LUAVALUE)
    ret = getLuaValue(L, key, LUA_TTABLE);
    if (ret) {
        lua_unlock(L);
        return ret;
    }
#endif  //SAVE_LUAVALUE

    jstring s = (*env)->NewStringUTF(env, key);
    ret = (*env)->NewObject(env, LuaTable, LuaTable_C, (jlong)L, s);
    FREE(env, s);
    lua_unlock(L);
    return ret;
}

jobject newLuaFunction(JNIEnv *env, lua_State *L, int idx)
{
    lua_lock(L);
    const char * key = copyValueToGNV(L, idx);
    jobject ret;

#if defined(SAVE_LUAVALUE)
    ret = getLuaValue(L, key, LUA_TFUNCTION);
    if (ret) {
        lua_unlock(L);
        return ret;
    }
#endif  //SAVE_LUAVALUE

    jstring s = (*env)->NewStringUTF(env, key);
    ret = (*env)->NewObject(env, LuaFunction, LuaFunction_C, (jlong)L, s);
    FREE(env, s);
    lua_unlock(L);
    return ret;
}

jobject newLuaUserdata(JNIEnv *env, lua_State *L, int idx, UDjavaobject ud)
{
    if (isJavaUserdata(ud))
    {
        if (ud->isStrongRef && !ud->isSetKey)
        {
            lua_lock(L);
            jstring nk = (*env)->NewStringUTF(env, copyValueToGNV(L, idx));
            lua_unlock(L);
            (*env)->SetObjectField(env, ud->jobj, LuaUserdata_nativeGlobalKey, nk);
            FREE(env, nk);
            ud->isSetKey = 1;
        }
        return ud->jobj;
    }
    return (*env)->NewObject(env, LuaUserdata, LuaUserdata_C,
                                  (jlong)L, NULL);
}

jobject newLuaThread(JNIEnv *env, lua_State *L, int idx)
{
    lua_lock(L);
    jstring s = (*env)->NewStringUTF(env, copyValueToGNV(L, idx));
    jobject ret = (*env)->NewObject(env, LuaThread, LuaThread_C, (jlong)L, s);
    FREE(env, ret);
    lua_unlock(L);
    return ret;
}

jobject toJavaValue(JNIEnv *env, lua_State *L, int idx)
{
    jobject result = NULL;
    lua_lock(L);
    switch (lua_type(L, idx))
    {
    case LUA_TNUMBER:
        result = newLuaNumber(env, (double)lua_tonumber(L, idx));
        break;
    case LUA_TNIL:
        result = Lua_NIL;
        break;
    case LUA_TBOOLEAN:
        result = lua_toboolean(L, idx) ? Lua_TRUE : Lua_FALSE;
        break;
    case LUA_TSTRING:
        result = newLuaString(env, lua_tostring(L, idx));
        break;
    case LUA_TTABLE:
        result = newLuaTable(env, L, idx);
        break;
    case LUA_TFUNCTION:
        result = newLuaFunction(env, L, idx);
        break;
    case LUA_TUSERDATA:
    case LUA_TLIGHTUSERDATA:
        result = newLuaUserdata(env, L, idx, (UDjavaobject)lua_touserdata(L, idx));
        break;
    case LUA_TTHREAD:
        result = newLuaThread(env, L, idx);
        break;
    }
    lua_unlock(L);
    return result;
}

jobjectArray newLuaValueArrayFromStack(JNIEnv *env, lua_State *L, int count, int stackoffset)
{
    lua_lock(L);
    count = count < 0 ? 0 : count;
    jobjectArray p = (*env)->NewObjectArray(env, count, LuaValue, NULL);
    int i;
    for (i = 0; i < count; i++)
    {
        jobject v = toJavaValue(env, L, i + stackoffset);
        (*env)->SetObjectArrayElement(env, p, (jsize)i, v);
        FREE(env, v);
    }
    lua_unlock(L);
    return p;
}

void pushUserdataFromJUD(JNIEnv *env, lua_State *L, jobject obj)
{
    lua_lock(L);
    jstring lcn = (jstring)(*env)->GetObjectField(env, obj, LuaUserdata_luaclassName);
    const char *luaclassname = GetString(env, lcn);

    UDjavaobject ud = (UDjavaobject)lua_newuserdata(L, sizeof(javaUserdata));
    ud->isStrongRef = 0;
    ud->isSetKey = 0;
    ud->jobj = GLOBAL(env, obj);
    // todo 将udname改为带缓存的名称、或使用malloc拼接字符串
    const char *udname = lua_pushfstring(L, METATABLE_FORMAT, luaclassname);
    ReleaseChar(env, lcn, luaclassname);
    FREE(env, lcn);
    lua_pop(L, 1);
    ud->name = udname;

    luaL_getmetatable(L, udname);
    lua_setmetatable(L, -2);
    lua_unlock(L);
}

void pushJavaValue(JNIEnv *env, lua_State *L, jobject obj)
{
    lua_lock(L);
    if (!obj)
    {
        lua_pushnil(L);
        lua_unlock(L);
        return;
    }
    int type = (int)(*env)->CallIntMethod(env, obj, LuaValue_type);
    double num;
    jstring string;
    const char *str;
    int idx;
    switch (type)
    {
    case LUA_TNUMBER:
        num = (double)(*env)->GetDoubleField(env, obj, LuaNumber_value);
        if (num == (int)num) 
            lua_pushinteger(L, (lua_Integer)num);
        else
            lua_pushnumber(L, (lua_Number)num);
        break;
    case LUA_TNIL:
        lua_pushnil(L);
        break;
    case LUA_TBOOLEAN:
        lua_pushboolean(L, (*env)->GetBooleanField(env, obj, LuaBoolean_value));
        break;
    case LUA_TSTRING:
        string = (jstring)(*env)->GetObjectField(env, obj, LuaString_value);
        str = GetString(env, string);
        lua_pushstring(L, str);
        ReleaseChar(env, string, str);
        FREE(env, string);
        break;
    case LUA_TUSERDATA:
        string = (jstring)(*env)->CallObjectMethod(env, obj, LuaValue_nativeGlobalKey);
        str = GetString(env, string);
        /// see LuaUserdata#newUserdata 由java创建的userdata，idx为LUA_REGISTRYINDEX
        if (isGlobal(str))
            pushUserdataFromJUD(env, L, obj);
        else
            getValueFromGNV(L, str, type);
        ReleaseChar(env, string, str);
        FREE(env, string);
        break;
    default:
        string = (jstring)(*env)->CallObjectMethod(env, obj, LuaValue_nativeGlobalKey);
        str = GetString(env, string);
        getValueFromGNV(L, str, type);
        ReleaseChar(env, string, str);
        FREE(env, string);
        break;
    }
    lua_unlock(L);
}

int pushJavaArray(JNIEnv *env, lua_State *L, jobjectArray arr)
{
    lua_lock(L);
    int len = arr ? GetArrLen(env, arr) : 0;

    if (len == 0)
        return 0;
    int i;
    for (i = 0; i < len; i++)
    {
        jobject jo = (*env)->GetObjectArrayElement(env, arr, i);
        pushJavaValue(env, L, jo);
        FREE(env, jo);
    }
    lua_unlock(L);
    return len;
}

void throwInvokeError(JNIEnv *env, const char *errmsg)
{
    jthrowable t = (*env)->ExceptionOccurred(env);
    if (t) {
        ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        // t = (*env)->NewObject(env, InvokeError, InvokeError_CT, (*env)->NewStringUTF(env, errmsg), t);
    }
    if (!InvokeError)
        InvokeError = GLOBAL(env, findTypeClass(env, "exception/InvokeError"));
    
    (*env)->ThrowNew(env, InvokeError, errmsg);
    // t = (*env)->NewObject(env, InvokeError, InvokeError_C, (*env)->NewStringUTF(env, errmsg));
    // (*env)->Throw(env, t);
}

void throwRuntimeError(JNIEnv *env, const char * msg)
{
    jthrowable t = (*env)->ExceptionOccurred(env);
    if (t) {
        ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
    }
    if (!RuntimeException)
        RuntimeException = GLOBAL(env, (*env)->FindClass(env, "java/lang/RuntimeException"));
    
    (*env)->ThrowNew(env, RuntimeException, msg);
}

void callbackLuaGC(JNIEnv *env, lua_State *L)
{
    (*env)->CallStaticVoidMethod(env, Globals, Globals__onLuaGC, (jlong)L);
    (*env)->ExceptionClear(env);
}

int getEnv(JNIEnv **out)
{
    int needDetach = 0;
    if ((*g_jvm)->GetEnv(g_jvm, (void**)out, JNI_VERSION_1_4) < 0 || !(*out))
    {
        (*g_jvm)->AttachCurrentThread(g_jvm, out, NULL);
        needDetach = 1;
    }
    return needDetach;
}

void detachEnv()
{
    (*g_jvm)->DetachCurrentThread(g_jvm);
}