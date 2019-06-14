/**
 * Created by Xiong.Fangyu 2019/02/22
 */

#include "luajapi.h"
#include "lgc.h"
#include "debug_info.h"
#include "mlog.h"
#include "luasocket.h"
#include <time.h>
#include "m_mem.h"
#include "compiler.h"
#include "saes.h"
#include "llimits.h"

extern JavaVM * g_jvm;
jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv *env = NULL;

    jint result = -1;
    if ((*vm)->GetEnv(vm, (void **)&env, JNI_VERSION_1_4) != JNI_OK)
    {
        return JNI_ERR;
    }
    jclass cls = (*env)->FindClass(env, JAVA_PATH "LuaCApi");
    if (!cls)
    {
        return JNI_ERR;
    }

    int len = sizeof(jni_methods) / sizeof(jni_methods[0]);
    LOGI("on load");
    if ((*env)->RegisterNatives(env, cls, jni_methods, len) < 0)
    {
        LOGE("on load error");
        return JNI_ERR;
    }
    FREE(env, cls);
    initJavaInfo(env);
    g_jvm = vm;
    return JNI_VERSION_1_4;
}

jboolean jni_check32bit(JNIEnv *env, jobject jobj)
{
    return sizeof(size_t) == 4;
}

jboolean jni_isSAESFile(JNIEnv *env, jobject jobj, jstring path)
{
    const char *file = GetString(env, path);
    int r = check_file(file);
    ReleaseChar(env, path, file);
    return r;
}

// --------------------------define field--------------------------
extern jclass LuaValue;
// --------------------------define private--------------------------
/**
 * 错误处理函数
 */
static int error_func_traceback(lua_State *);
/**
 * 获取pcall中处理错误的函数的栈位置
 */
static int getErrorFunctionIndex(lua_State *L);

// --------------------------end--------------------------

/// ------------------------------------------------------------------
// ---------------------------------gc--------------------------------
/// ------------------------------------------------------------------

#define GC_OFFSET_TIME (CLOCKS_PER_SEC >> 3)
static clock_t last_gc_time = 0;
static int gc_offset_time = GC_OFFSET_TIME;

static void gc_cb(lua_State *L, int type) {
    if (gc_offset_time <= 0) return;
    if (type >= GCSatomic && type <= GCSsweep) {
        clock_t now = clock();
        if (last_gc_time != 0) {
            int offset = now - last_gc_time;
            if (offset < gc_offset_time) return;
        }
        last_gc_time = now;
        JNIEnv *env;
        int d = getEnv(&env);
        callbackLuaGC(env, L);
        if (d) detachEnv();
    }
}

void jni_setGcOffset(JNIEnv *env, jobject jobj, int offset) {
    gc_offset_time = offset * GC_OFFSET_TIME;
}

/// ------------------------------------------------------------------
/// -------------------------------debug------------------------------
/// ------------------------------------------------------------------
#if defined(J_API_INFO)
static size_t all_size = 0;
static void *m_alloc (void *ud, void *ptr, size_t osize, size_t nsize) {
  size_t * sp = (size_t *)ud;
  if (nsize == 0) {
    free(ptr);
    if (ptr) {
        *sp = *sp - osize;
        all_size -= osize;
    }
    return NULL;
  }
  else {
    void * nb = realloc(ptr, nsize);
    if (nb) {
        size_t of = (ptr) ? (nsize - osize) : nsize;
        *sp = *sp + of;
        all_size += of;
    }
    return nb;
  }
}

static size_t all_global_obj = 0;
jobject _global(JNIEnv * env, jobject obj) {
    all_global_obj ++;
    return (*env)->NewGlobalRef(env, obj);
}

void _unglobal(JNIEnv * env, jobject obj) {
    all_global_obj --;
    (*env)->DeleteGlobalRef(env, obj);
}
#endif

jlong jni_lvmMemUse(JNIEnv * env, jobject jobj, jlong L) {
    #if defined(J_API_INFO)
    lua_State *LS = (lua_State *)L;
    return *(size_t *)(G(LS)->ud);
    #else
    return 0;
    #endif
}

jlong jni_allLvmMemUse(JNIEnv *env, jobject jobj) {
    #if defined(J_API_INFO)
    return (jlong) (all_size + m_mem_use());
    #else
    return 0;
    #endif
}

jlong jni_globalObjectSize(JNIEnv *env, jobject jobj) {
    #if defined(J_API_INFO)
    return (jlong) all_global_obj;
    #else
    return 0;
    #endif
}

void jni_logMemoryInfo(JNIEnv *env, jobject jobj) {
#if defined(J_API_INFO) && defined(MEM_INFO)
    m_log_mem_infos();
#endif
}

/// ------------------------------------------------------------------
/// ------------------------------L State-----------------------------
/// ------------------------------------------------------------------
jlong jni_createLState(JNIEnv *env, jobject jobj, jboolean debug)
{
    #if defined(J_API_INFO)
    size_t* ud = (size_t*) m_malloc(NULL, 0, sizeof(size_t));
    *ud = 0;
    lua_State *L = luaL_newstate1(m_alloc, ud);
    #else
    lua_State *L = luaL_newstate();
    #endif
    lua_lock(L);
    L->l_G->gc_callback = NULL;
    luaL_openlibs(L);
    if (debug)
        luaopen_socket_core(L);
    lua_pushcfunction(L, error_func_traceback);
    lua_setglobal(L, ERROR_FUN);
    lua_getglobal(L, ERROR_FUN);
    initlog(env);
    init_cache(L);

    lua_getglobal(L, LUA_LOADLIBNAME);      //-1 package table
    luaL_getsubtable(L, -1, "searchers");   //-1 package.searchers table; -2 package table
    lua_pushcfunction(L, searcher_java);    //-1: fun;-2 package.searchers table; -3 package table
    lua_rawseti(L, -2, lua_objlen(L, -2) + 1);
    lua_pop(L, 2);
    if (gc_offset_time > 0)
        G(L)->gc_callback = gc_cb;
    lua_unlock(L);

    return (jlong)L;
}

void jni_close(JNIEnv *env, jobject jobj, jlong L)
{
    lua_State *LS = (lua_State *)L;
    lua_close(LS);
    #if defined(J_API_INFO)
    m_malloc(G(LS)->ud, sizeof(size_t), 0);
    cj_log();
    #endif
}

/// ------------------------------------------------------------------
/// --------------------------stack for java--------------------------
/// ------------------------------------------------------------------
jint jni_registerIndex(JNIEnv *env, jobject jobj)
{
    return (jint)LUA_REGISTRYINDEX;
}

jobjectArray jni_dumpStack(JNIEnv *env, jobject jobj, jlong L)
{
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    int size = lua_gettop(LS);
    jobjectArray arr = (*env)->NewObjectArray(env, (jsize)size, LuaValue, NULL);
    int index, i;
    for (index = size; index > 0; --index)
    {
        i = size - index;
        jobject v = toJavaValue(env, LS, index);
        (*env)->SetObjectArrayElement(env, arr, i, v);
        FREE(env, v);
    }
    lua_unlock(LS);
    return arr;
}

void jni_removeStack(JNIEnv *env, jobject jobj, jlong L, jint idx)
{
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    if (idx == lua_gettop(LS))
    {
        lua_pop(LS, 1);
    }
    else
    {
        lua_remove(LS, (int)idx);
    }
    lua_unlock(LS);
}

void jni_pop(JNIEnv *env, jobject jobj, jlong L, jint c)
{
    lua_State *LS = (lua_State *)L;
    lua_pop(LS, (int)c);
}

jint jni_getTop(JNIEnv *env, jobject jobj, jlong L)
{
    return (jint)lua_gettop((lua_State *)L);
}

// --------------------------function--------------------------
jobjectArray jni_invoke(JNIEnv *env, jobject jobj, jlong L, jstring function, jobjectArray params, jint rc)
{
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);
    int erridx = getErrorFunctionIndex(LS);
    int oldTop = lua_gettop(LS);
    const char *fk = GetString(env, function);
    getValueFromGNV(LS, fk, LUA_TFUNCTION);
    ReleaseChar(env, function, fk);
    if (lua_isnil(LS, -1))
    {
        throwInvokeError(env, "function is destroyed.");
        lua_pop(LS, 1);
        lua_unlock(LS);
        return NULL;
    }

    luaC_checkGC(LS);
    int len = pushJavaArray(env, LS, params);
    int ret = lua_pcall(LS, len, (int)rc, erridx);
    if (ret != 0)
    {
        const char *errMsg;
        if (lua_isstring(LS, -1))
            errMsg = lua_tostring(LS, -1);
        lua_settop(LS, oldTop);
        throwInvokeError(env, errMsg);
        lua_unlock(LS);
        return NULL;
    }
    int returnCount = lua_gettop(LS) - oldTop;
    if (returnCount == 0)
    {
        lua_settop(LS, oldTop);
        // lua_pop(LS, 1);
        lua_unlock(LS);
        return NULL;
    }
    int i;
    jobjectArray r = (*env)->NewObjectArray(env, returnCount, LuaValue, NULL);
    for (i = returnCount - 1; i >= 0; i--)
    {
        jobject v = toJavaValue(env, LS, oldTop + i + 1);
        (*env)->SetObjectArrayElement(env, r, i, v);
        FREE(env, v);
    }
    lua_settop(LS, oldTop);
    lua_unlock(LS);
    return r;
}

// --------------------------error function--------------------------
static inline int getErrorFunctionIndex(lua_State *L)
{
    if (lua_iscfunction(L, 1) && lua_tocfunction(L, 1) == error_func_traceback)
    {
        return 1;
    }
    lua_getglobal(L, ERROR_FUN);
    return lua_gettop(L);
}

static int error_func_traceback(lua_State *L)
{
    if (!lua_isstring(L, 1)) //错误信息不是string类型
        return 1;
    lua_getglobal(L, LUA_DBLIBNAME); //获取debug函数，压入栈中
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        lua_pushstring(L, "no debug lib!");
        return 1;
    }
    lua_getfield(L, -1, "traceback"); //获取debug.tracenack函数,压入栈中
    if (!lua_isfunction(L, -1))
    {
        lua_pop(L, 2);
        lua_pushstring(L, "no debug.traceback method!");
        return 1;
    }
    lua_pushvalue(L, 1);   // 传递错误信息
    lua_pushinteger(L, 2); // skip this function and traceback
    lua_call(L, 2, -1);    // 调用debug.traceback
    return 1;
}