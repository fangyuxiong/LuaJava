//
// Created by Xiong.Fangyu 2019/03/13.
//

#include "luajapi.h"
#include "debug_info.h"
#include "luaconf.h"
#include "m_mem.h"
#include "llimits.h"

extern jclass LuaValue;
static jclass Entrys = NULL;

jstring jni_createTable(JNIEnv *env, jobject jobj, jlong L)
{
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    lua_newtable(LS);
    const char *key = copyValueToGNV(LS, -1);
    lua_pop(LS, 1);
    lua_unlock(LS);
    return (*env)->NewStringUTF(env, key);
}

jint jni_getTableSize(JNIEnv *env, jobject jobj, jlong L, jstring table)
{
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    const char *key = GetString(env, table);
    getValueFromGNV(LS, key, LUA_TTABLE);
    ReleaseChar(env, table, key);
    jint size = (jint)lua_objlen(LS, -1);
    lua_pop(LS, 1);
    lua_unlock(LS);
    return size;
}

void jni_setTableNumber(JNIEnv *env, jobject jobj, jlong L, jstring table, jint k, jdouble n)
{
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    const char *key = GetString(env, table);
    getValueFromGNV(LS, key, LUA_TTABLE);
    ReleaseChar(env, table, key);
    lua_pushinteger(LS, (lua_Integer)k);
    lua_pushnumber(LS, (lua_Number)n);
    lua_rawset(LS, -3);
    lua_pop(LS, 1);
    lua_unlock(LS);
}
void jni_setTableBoolean(JNIEnv *env, jobject jobj, jlong L, jstring table, jint k, jboolean v)
{
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    const char *key = GetString(env, table);
    getValueFromGNV(LS, key, LUA_TTABLE);
    ReleaseChar(env, table, key);
    lua_pushinteger(LS, (lua_Integer)k);
    lua_pushboolean(LS, (int)v);
    lua_rawset(LS, -3);
    lua_pop(LS, 1);
    lua_unlock(LS);
}
void jni_setTableString(JNIEnv *env, jobject jobj, jlong L, jstring table, jint k, jstring v)
{
    const char *str = GetString(env, v);
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    const char *key = GetString(env, table);
    getValueFromGNV(LS, key, LUA_TTABLE);
    ReleaseChar(env, table, key);
    lua_pushinteger(LS, (lua_Integer)k);
    lua_pushstring(LS, str);
    lua_rawset(LS, -3);
    lua_pop(LS, 1);
    ReleaseChar(env, v, str);
    lua_unlock(LS);
}
void jni_setTableNil(JNIEnv *env, jobject jobj, jlong L, jstring table, jint k)
{
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    const char *key = GetString(env, table);
    getValueFromGNV(LS, key, LUA_TTABLE);
    ReleaseChar(env, table, key);
    lua_pushinteger(LS, (lua_Integer)k);
    lua_pushnil(LS);
    lua_rawset(LS, -3);
    lua_pop(LS, 1);
    lua_unlock(LS);
}
void jni_setTableChild(JNIEnv *env, jobject jobj, jlong L, jstring table, jint k, jobject child, jint lt)
{
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    const char *key = GetString(env, table);
    getValueFromGNV(LS, key, LUA_TTABLE);
    ReleaseChar(env, table, key);
    lua_pushinteger(LS, (lua_Integer)k);
    if (lt < 0)
        pushUserdataFromJUD(env, LS, child);
    else
    {
        const char *ckey = GetString(env, child);
        getValueFromGNV(LS, ckey, lt);
        ReleaseChar(env, child, ckey);
    }
    lua_rawset(LS, -3);
    lua_pop(LS, 1);
    lua_unlock(LS);
}

void jni_setTableSNumber(JNIEnv *env, jobject jobj, jlong L, jstring table, jstring k, jdouble v)
{
    const char *key = GetString(env, k);
    const char *tk = GetString(env, table);
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    if (isGlobal(tk))
    {
        lua_pushnumber(LS, (lua_Number)v);
        lua_setglobal(LS, key);
    }
    else
    {
        getValueFromGNV(LS, tk, LUA_TTABLE);
        lua_pushstring(LS, key);
        lua_pushnumber(LS, (lua_Number)v);
        lua_rawset(LS, -3);
        lua_pop(LS, 1);
    }
    ReleaseChar(env, table, tk);
    ReleaseChar(env, k, key);
    lua_unlock(LS);
}
void jni_setTableSBoolean(JNIEnv *env, jobject jobj, jlong L, jstring table, jstring k, jboolean v)
{
    const char *key = GetString(env, k);
    const char *tk = GetString(env, table);
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    if (isGlobal(tk))
    {
        lua_pushboolean(LS, (int)v);
        lua_setglobal(LS, key);
    }
    else
    {
        getValueFromGNV(LS, tk, LUA_TTABLE);
        lua_pushstring(LS, key);
        lua_pushboolean(LS, (int)v);
        lua_rawset(LS, -3);
        lua_pop(LS, 1);
    }
    ReleaseChar(env, table, tk);
    ReleaseChar(env, k, key);
    lua_unlock(LS);
}
void jni_setTableSString(JNIEnv *env, jobject jobj, jlong L, jstring table, jstring k, jstring v)
{
    const char *tk = GetString(env, table);
    const char *key = GetString(env, k);
    const char *value = GetString(env, v);
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    if (isGlobal(tk))
    {
        lua_pushstring(LS, value);
        lua_setglobal(LS, key);
    }
    else
    {
        getValueFromGNV(LS, tk, LUA_TTABLE);
        lua_pushstring(LS, key);
        lua_pushstring(LS, value);
        lua_rawset(LS, -3);
        lua_pop(LS, 1);
    }
    ReleaseChar(env, table, tk);
    ReleaseChar(env, k, key);
    ReleaseChar(env, v, value);
    lua_unlock(LS);
}
void jni_setTableSNil(JNIEnv *env, jobject jobj, jlong L, jstring table, jstring k)
{
    const char *tk = GetString(env, table);
    const char *key = GetString(env, k);
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    if (isGlobal(tk))
    {
        lua_pushnil(LS);
        lua_setglobal(LS, key);
    }
    else
    {
        getValueFromGNV(LS, tk, LUA_TTABLE);
        lua_pushstring(LS, key);
        lua_pushnil(LS);
        lua_rawset(LS, -3);
        lua_pop(LS, 1);
    }
    ReleaseChar(env, table, tk);
    ReleaseChar(env, k, key);
    lua_unlock(LS);
}
void jni_setTableSChild(JNIEnv *env, jobject jobj, jlong L, jstring table, jstring k, jobject child, jint lt)
{
    const char *tk = GetString(env, table);
    const char *key = GetString(env, k);
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    if (isGlobal(tk))
    {
        if (lt < 0)
            pushUserdataFromJUD(env, LS, child);
        else 
        {
            const char *ck = GetString(env, child);
            getValueFromGNV(LS, ck, lt);
            ReleaseChar(env, child, ck);
        }
        lua_setglobal(LS, key);
    }
    else
    {
        getValueFromGNV(LS, tk, LUA_TTABLE);
        lua_pushstring(LS, key);
        if (lt < 0)
            pushUserdataFromJUD(env, LS, child);
        else 
        {
            const char *ck = GetString(env, child);
            getValueFromGNV(LS, ck, lt);
            ReleaseChar(env, child, ck);
        }
        lua_rawset(LS, -3);
        lua_pop(LS, 1);
    }
    ReleaseChar(env, table, tk);
    ReleaseChar(env, k, key);
    lua_unlock(LS);
}

jobject jni_getTableValue(JNIEnv *env, jobject jobj, jlong L, jstring table, jint k)
{
    const char *tk = GetString(env, table);
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    getValueFromGNV(LS, tk, LUA_TTABLE);
    ReleaseChar(env, table, tk);
    lua_pushinteger(LS, (lua_Integer)k);
    lua_rawget(LS, -2);
    lua_remove(LS, -2);
    jobject ret = toJavaValue(env, LS, -1);
    lua_pop(L, 1);
    lua_unlock(LS);

    return ret;
}

jobject jni_getTableSValue(JNIEnv *env, jobject jobj, jlong L, jstring table, jstring k)
{
    const char *tk = GetString(env, table);
    const char *key = GetString(env, k);
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    if (isGlobal(tk))
    {
        lua_getglobal(LS, key);
    }
    else
    {
        getValueFromGNV(LS, tk, LUA_TTABLE);
        lua_pushstring(LS, key);
        lua_rawget(LS, -2);
        lua_remove(LS, -2);
    }
    ReleaseChar(env, table, tk);
    ReleaseChar(env, k, key);
    jobject ret = toJavaValue(env, LS, -1);
    lua_pop(L, 1);
    lua_unlock(LS);

    return ret;
}

jboolean jni_startTraverseTable(JNIEnv *env, jobject jobj, jlong L, jstring table)
{
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    const char *tk = GetString(env, table);
    int globalTable = isGlobal(tk);
    if (globalTable)
        lua_pushglobaltable(LS);
    else
        getValueFromGNV(LS, tk, LUA_TTABLE);
    ReleaseChar(env, table, tk);
    if (lua_isnil(LS, -1)) {
        lua_pop(LS, 1);
        return (jboolean)0;
    }
    lua_pushnil(LS);
    lua_unlock(LS);
    return (jboolean)1;
}

static inline int isValidKeyForGlobal(lua_State *LS)
{
    if (lua_isstring(LS, -2))
    {
        const char *key = lua_tostring(LS, -2);
        if (!strcmp(key, GNV) || !strcmp(key, "load"))
        {
            lua_pop(LS, 1);
            return 0;
        }
    }
    return 1;
}

jobjectArray jni_nextEntry(JNIEnv *env, jobject jobj, jlong L, jboolean isGlobal)
{
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    while (lua_next(LS, -2)) {         // xxx table
        if (isGlobal && !isValidKeyForGlobal(LS))
            continue;
        jobject key = toJavaValue(env, LS, -2);
        jobject value = toJavaValue(env, LS, -1);
        lua_pop(LS, 1);             // xxx table
        jobjectArray ret = (*env)->NewObjectArray(env, 2, LuaValue, NULL);
        (*env)->SetObjectArrayElement(env, ret, 0, key);
        (*env)->SetObjectArrayElement(env, ret, 1, value);
        FREE(env, key);
        FREE(env, value);
        lua_unlock(LS);
        return ret;
    }
    lua_pushnil(LS);// nil, table
    lua_unlock(LS);
    return NULL;
}

void jni_endTraverseTable(JNIEnv *env, jobject jobj, jlong L)
{
    lua_pop((lua_State *)L, 2);
}

jobject jni_getTableEntry(JNIEnv *env, jobject jobj, jlong L, jstring table)
{
    typedef struct JLink
    {
        jobject key;
        jobject value;
        struct JLink *next;
    } JLink;

    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    const char *tk = GetString(env, table);
    int globalTable = isGlobal(tk);
    if (globalTable)
        lua_pushglobaltable(LS);
    else
        getValueFromGNV(LS, tk, LUA_TTABLE);
    ReleaseChar(env, table, tk);
    lua_pushnil(LS);                            // nil table
    int num = 0;

    JLink *head = NULL;
    JLink *pre = NULL;
    JLink *node = NULL;
    while (lua_next(LS, -2))                    // value key table
    {
        if (globalTable && !isValidKeyForGlobal(LS))
            continue;
        
        jobject key = toJavaValue(env, LS, -2);
        jobject value = toJavaValue(env, LS, -1);
        lua_pop(LS, 1);

        node = (JLink *)m_malloc(NULL, 0, sizeof(JLink));
        node->key = key;
        node->value = value;

        if (num++ == 0)
        {
            head = node;
            pre = node;
        }
        else
        {
            pre->next = node;
            pre = node;
        }
    }
    lua_pop(LS, 1);
    lua_unlock(LS);

    jobjectArray keysarr = (*env)->NewObjectArray(env, (jsize)num, LuaValue, NULL);
    jobjectArray valuearr = (*env)->NewObjectArray(env, (jsize)num, LuaValue, NULL);
    int i;
    node = head;
    for (i = 0; i < num; i++)
    {
        (*env)->SetObjectArrayElement(env, keysarr, i, node->key);
        (*env)->SetObjectArrayElement(env, valuearr, i, node->value);
        pre = node;
        node = node->next;
        FREE(env, pre->key);
        FREE(env, pre->value);
        m_malloc(pre, sizeof(JLink), 0);
    }

    
    if (!Entrys)
        Entrys = GLOBAL(env, findTypeClass(env, "LuaTable$Entrys"));

    jmethodID con = findConstructor(env, Entrys, "[" LUAVALUE_CLASS "[" LUAVALUE_CLASS);
    return (*env)->NewObject(env, Entrys, con, keysarr, valuearr);
}

void copyTable(lua_State *L, int src, int desc)
{
    lua_lock(L);
    src = src < 0 ? lua_gettop(L) + src + 1 : src;
    desc = desc < 0 ? lua_gettop(L) + desc + 1 : desc;
    lua_pushnil(L);
    while (lua_next(L, src))
    {
        if (lua_isstring(L, -2))
        {
            const char *key = lua_tostring(L, -2);
            if (key[0] == '_' && key[1] == '_')
            {
                lua_pop(L, 1);
                continue;
            }
        }
        lua_pushvalue(L, -2);
        lua_pushvalue(L, -2);
        lua_rawset(L, desc);
        lua_pop(L, 1);
    }
    lua_unlock(L);
}