//
// Created by Xiong.Fangyu 2019/03/13.
//

#include "luajapi.h"
#include "lgc.h"
#include "m_mem.h"
#include "llimits.h"

#define S_DEFAULT_SIG "(J[" LUAVALUE_CLASS ")[" LUAVALUE_CLASS
static void pushStaticClosure(JNIEnv *env, lua_State *L, jclass clz, jmethodID m, int pc);
static int executeJavaStaticFunction(lua_State *L);

void jni_registerStaticClassSimple(JNIEnv *env, jobject jobj, jlong L, jstring jn, jstring ln, jstring lpcn, jobjectArray methods)
{
    const char *jclassname = GetString(env, jn);
    jclass clz = (jclass)cj_get(jclassname);
    if (!clz) {
        clz = (*env)->FindClass(env, jclassname);
        if (!clz) {
            char *errorstr = joinstr("cannot find class ", jclassname);
            ReleaseChar(env, jn, jclassname);
            throwRuntimeError(env, errorstr);
            m_malloc(errorstr, (strlen(errorstr) + 1) * sizeof(char), 0);
            return;
        }
        clz = GLOBAL(env, clz);
        cj_put(jclassname, clz);
    }
    ReleaseChar(env, jn, jclassname);
    
    int len = GetArrLen(env, methods);
    int i;

    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    lua_createtable(LS, 0, len);

    const char *parent_name;
    if (lpcn)
    {
        parent_name = GetString(env, lpcn);
        lua_getglobal(LS, parent_name);
        if (lua_istable(LS, -1))
            copyTable(LS, -1, -2);
        lua_pop(LS, 1);
        ReleaseChar(env, lpcn, parent_name);
    }

    const char *methodName;
    jstring jmethodName;
    jmethodID m;
    for (i = 0; i < len; i++)
    {
        jmethodName = (jstring)(*env)->GetObjectArrayElement(env, methods, i);
        methodName = GetString(env, jmethodName);
        m = (*env)->GetStaticMethodID(env, clz, methodName, S_DEFAULT_SIG);
        if (!m)
        {
            ReleaseChar(env, jmethodName, methodName);
            FREE(env, jmethodName);
            throwRuntimeError(env, "Find method error!");
            lua_unlock(LS);
            return;
        }
        lua_pushstring(LS, methodName);
        pushStaticClosure(env, LS, clz, m, -1);
        lua_rawset(LS, -3);
        ReleaseChar(env, jmethodName, methodName);
        FREE(env, jmethodName);
    }
    const char *lname = GetString(env, ln);
    lua_setglobal(LS, lname);
    ReleaseChar(env, ln, lname);
    lua_unlock(LS);
}

void jni_registerStaticClass(JNIEnv *env, jobject jobj, jlong L, jstring jn, jstring ln, jobjectArray methods, jobjectArray lmn, jintArray pcs)
{
    const char *jclassname = GetString(env, jn);
    jclass clz = (jclass)cj_get(jclassname);
    if (!clz) {
        clz = (*env)->FindClass(env, jclassname);
        if (!clz) {
            char *errorstr = joinstr("cannot find class ", jclassname);
            ReleaseChar(env, jn, jclassname);
            throwRuntimeError(env, errorstr);
            m_malloc(errorstr, (strlen(errorstr) + 1) * sizeof(char), 0);
            return;
        }
        clz = GLOBAL(env, clz);
        cj_put(jclassname, clz);
    }
    ReleaseChar(env, jn, jclassname);

    int len = GetArrLen(env, methods);
    if (len != GetArrLen(env, lmn)) {
        throwRuntimeError(env, "java method array and lua method array must have same length!");
        return;
    }
    int i;

    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    lua_createtable(LS, 0, len);

    jint *paramCount;
    int plen = 0;
    if (pcs)
        plen = GetArrLen(env, pcs);
    if (plen)
    {
        if (plen != len)
        {
            throwRuntimeError(env, "param count array can be null, otherwise java method array and param count array must have same length!");
            lua_unlock(LS);
            return;
        }
        paramCount = (*env)->GetIntArrayElements(env, pcs, 0);
    }

    const char *methodName;
    jstring jmethodName;
    const char *lmethodName;
    jstring lmethodNameStr;
    jmethodID m;
    for (i = 0; i < len; i++)
    {
        jmethodName = (jstring)(*env)->GetObjectArrayElement(env, methods, i);
        methodName = GetString(env, jmethodName);
        m = (*env)->GetStaticMethodID(env, clz, methodName, S_DEFAULT_SIG);
        if (!m)
        {
            if (plen)
                (*env)->ReleaseIntArrayElements(env, pcs, paramCount, 0);
            ReleaseChar(env, jmethodName, methodName);
            FREE(env, jmethodName);
            throwRuntimeError(env, "Find method error!");
            lua_unlock(LS);
            return;
        }
        lmethodNameStr = (jstring)(*env)->GetObjectArrayElement(env, lmn, i);
        lmethodName = GetString(env, lmethodNameStr);

        lua_pushstring(LS, lmethodName);
        pushStaticClosure(env, LS, clz, m, plen ? paramCount[i] : -1);
        lua_rawset(LS, -3);
        ReleaseChar(env, jmethodName, methodName);
        ReleaseChar(env, lmethodNameStr, lmethodName);
        FREE(env, jmethodName);
        FREE(env, lmethodNameStr);
    }
    if (plen)
        (*env)->ReleaseIntArrayElements(env, pcs, paramCount, 0);
    const char *lname = GetString(env, ln);
    lua_setglobal(LS, lname);
    ReleaseChar(env, ln, lname);
    lua_unlock(LS);
}

/**
 * 对应executeJavaStaticFunction
 */
static void pushStaticClosure(JNIEnv *env, lua_State *L, jclass clz, jmethodID m, int pc)
{
    UDJNIEnv udjni = (UDJNIEnv)lua_newuserdata(L, sizeof(JNIEnv *));
    *udjni = env;

    UDjclass udclz = (UDjclass)lua_newuserdata(L, sizeof(jclass));
    *udclz = clz;

    UDjmethod udm = (UDjmethod)lua_newuserdata(L, sizeof(jmethodID));
    *udm = m;

    lua_pushinteger(L, (lua_Integer)pc);
    lua_pushcclosure(L, executeJavaStaticFunction, 4);
}

/**
 * 对应pushStaticClosure
 * upvalue顺序为:1:UDJNIEnv, 
 *              2:UDjclass, 
 *              3:UDjmethod, 
 *              4:parmaCount
 */
static int executeJavaStaticFunction(lua_State *L)
{
    lua_lock(L);
    /// 第一个参数为JNI环境
    int idx = lua_upvalueindex(1);
    UDJNIEnv udenv = (UDJNIEnv)lua_touserdata(L, idx);

    /// 第二个参数为Java静态类
    idx = lua_upvalueindex(2);
    UDjclass udclz = (UDjclass)lua_touserdata(L, idx);

    /// 第三个参数为Java静态方法
    idx = lua_upvalueindex(3);
    UDjmethod udmethod = (UDjmethod)lua_touserdata(L, idx);

    /// 第四个参数为方法需要的参数个数
    /// -1表示可变个数
    int pc = lua_tointeger(L, lua_upvalueindex(4));
    if (pc == -1)
    {
        pc = lua_gettop(L) - 1; ///去掉栈底的table
    }

    if (!lua_istable(L, 1))
    {
        lua_pushstring(L, "use ':' instead of '.' to call method!!");
        lua_unlock(L);
        return lua_error(L);
    }

    JNIEnv *env = getuserdata(udenv);
    jobjectArray p = newLuaValueArrayFromStack(env, L, pc, 2);

    jobjectArray result = (jobjectArray)(*env)->CallStaticObjectMethod(env, getuserdata(udclz), getuserdata(udmethod), (jlong)L,  p);
    if ((*env)->ExceptionCheck(env))
    {
        ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        FREE(env, p);
        lua_pushstring(L, "exception throws in java methods!");
        lua_error(L);
        lua_unlock(L);
        return 1;
    }
    FREE(env, p);
    if (!result)
    {
        lua_settop(L, 1);
        lua_unlock(L);
        return 1;
    }
    luaC_checkGC(L);
    int rc = pushJavaArray(env, L, result);
    FREE(env, result);
    lua_unlock(L);
    return rc;
}

static inline void findOrCreateTable(lua_State *L, const char *name, int len)
{
    lua_getglobal(L, name);
    if (lua_isnil(L, -1))
    {
        lua_pop(L, 1);
        lua_createtable(L, 0, len);
        lua_pushvalue(L, -1);
        lua_setglobal(L, name);
    }
}

void jni_registerNumberEnum(JNIEnv *env, jobject jobj, jlong LS, jstring lcn, jobjectArray keys, jdoubleArray values)
{
    lua_State *L = (lua_State *)LS;
    lua_lock(L);
    const char *name = GetString(env, lcn);
    int len = GetArrLen(env, keys);
    findOrCreateTable(L, name, len);        // -1: table
    ReleaseChar(env, lcn, name);
    
    int i;
    jstring jk;
    const char *k;
    jdouble *vs = (*env)->GetDoubleArrayElements(env, values, 0);
    for(i = 0; i < len; i++)
    {
        jk = (*env)->GetObjectArrayElement(env, keys, i);
        k = GetString(env, jk);
        lua_pushstring(L, k);                   // -1:key --table
        ReleaseChar(env, jk, k);
        FREE(env, jk);
        lua_pushnumber(L, (lua_Number) vs[i]);  // -1:num --key-table
        lua_rawset(L, -3);                      // -1:table
    }
    lua_pop(L, 1);
    (*env)->ReleaseDoubleArrayElements(env, values, vs, 0);
    lua_unlock(L);
}

void jni_registerStringEnum(JNIEnv *env, jobject jobj, jlong LS, jstring lcn, jobjectArray keys, jobjectArray values)
{
    lua_State *L = (lua_State *)LS;
    lua_lock(L);
    const char *name = GetString(env, lcn);
    int len = GetArrLen(env, keys);
    findOrCreateTable(L, name, len);
    ReleaseChar(env, lcn, name);
    
    int i;
    jstring jk, jv;
    const char *k;
    const char *v;
    for(i = 0; i < len; i++)
    {
        jk = (*env)->GetObjectArrayElement(env, keys, i);
        k = GetString(env, jk);
        lua_pushstring(L, k);
        ReleaseChar(env, jk, k);
        FREE(env, jk);
        jv = (*env)->GetObjectArrayElement(env, values, i);
        v = GetString(env, jv);
        lua_pushstring(L, v);
        ReleaseChar(env, jv, v);
        FREE(env, jv);
        lua_rawset(L, -3);
    }
    lua_pop(L, 1);
    lua_unlock(L);
}