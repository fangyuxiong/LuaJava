//
// Created by Xiong.Fangyu 2019/03/13.
//

#include "juserdata.h"
#include "lgc.h"
#include "debug_info.h"
#include "m_mem.h"
#include "llimits.h"

/**
 * 注册ud
 * lcn: lua class name          nonnull
 * lpcn: lua parent class name  nullable
 * jcn: java class name         nonnull
 * jms: java methods            nullable
 * mc:  java methods count      >=0
 * lazy: 是否懒注册
 */
static void register_ud(JNIEnv *env, lua_State *L, const char *lcn, const char *lpcn, const char *jcn, char **jms, int mc, int lazy);


void jni_registerAllUserdata(JNIEnv *env, jobject jobj, jlong L, jobjectArray lcns, jobjectArray lpcns, jobjectArray jcns, jintArray mc, jbooleanArray lazy, jobjectArray ams)
{
    int len = GetArrLen(env, lcns);
    int i;
    int methodStartIndex = 0;
    int *mcs = (*env)->GetIntArrayElements(env, mc, 0);
    jboolean *lz = (*env)->GetBooleanArrayElements(env, lazy, 0);
    for(i = 0; i < len; i++)
    {
        jstring lcn = (*env)->GetObjectArrayElement(env, lcns, i);
        jstring lpcn = (*env)->GetObjectArrayElement(env, lpcns, i);
        jstring jcn = (*env)->GetObjectArrayElement(env, jcns, i);
        int methodCount = mcs[i];
        char **jms = NULL;
        if (methodCount > 0) {
            jms = get_methods_str(env, ams, methodCount, methodStartIndex);
            methodStartIndex += methodCount;
        }
        const char *_lcn = GetString(env, lcn);
        const char *_lpcn = GetString(env, lpcn);
        const char *_jcn = GetString(env, jcn);
        register_ud(env, (lua_State *)L, _lcn, _lpcn, _jcn, jms, methodCount, lz[i]);
        ReleaseChar(env, lcn, _lcn);
        FREE(env, lcn);
        ReleaseChar(env, jcn, _jcn);
        FREE(env, jcn);
        if (lpcn) {
            ReleaseChar(env, lpcn, _lpcn);
            FREE(env, lpcn);
        }
    }
    (*env)->ReleaseIntArrayElements(env, mc, mcs, 0);
    (*env)->ReleaseBooleanArrayElements(env, lazy, lz, 0);
}

void jni_registerUserdata(JNIEnv *env, jobject jobj, jlong L, jstring lcn, jstring lpcn, jstring jcn, jobjectArray jms) {
    const char *_lcn = GetString(env, lcn);
    const char *_lpcn = GetString(env, lpcn);
    const char *_jcn = GetString(env, jcn);
    int mc = GetArrLen(env, jms);
    char **_jms = NULL;
    if (mc > 0) {
        _jms = get_methods_str(env, jms, mc, 0);
    }
    register_ud(env, (lua_State *)L, _lcn, _lpcn, _jcn, _jms, mc, 0);
    ReleaseChar(env, lcn, _lcn);
    ReleaseChar(env, jcn, _jcn);
    if (lpcn) {
        ReleaseChar(env, lpcn, _lpcn);
    }
}

void jni_registerUserdataLazy(JNIEnv *env, jobject jobj, jlong L, jstring lcn, jstring lpcn, jstring jcn, jobjectArray jms) {
    const char *_lcn = GetString(env, lcn);
    const char *_lpcn = GetString(env, lpcn);
    const char *_jcn = GetString(env, jcn);
    int mc = GetArrLen(env, jms);
    char **_jms = NULL;
    if (mc > 0) {
        _jms = get_methods_str(env, jms, mc, 0);
    }
    register_ud(env, (lua_State *)L, _lcn, _lpcn, _jcn, _jms, mc, 1);
    ReleaseChar(env, lcn, _lcn);
    ReleaseChar(env, jcn, _jcn);
    if (lpcn) {
        ReleaseChar(env, lpcn, _lpcn);
    }
}

jobject jni_createUserdataAndSet(JNIEnv *env, jobject jobj, jlong L, jstring key, jstring lcn, jobjectArray p)
{
    const char *kstr = GetString(env, key);
    const char *name = GetString(env, lcn);
    lua_State *LS = (lua_State *)L;
    lua_lock(LS);

    lua_getglobal(LS, name);
    if (!lua_isfunction(LS, -1))
    {
        char * em = joinstr(name, "not registed!");
        throwRuntimeError(env, em);
        m_malloc(em, (strlen(em) + 1) * sizeof(char), 0);
        ReleaseChar(env, key, kstr);
        ReleaseChar(env, lcn, name);
        lua_pop(LS, 1);
        lua_unlock(LS);
        return NULL;
    }

    luaC_checkGC(LS);
    int c = pushJavaArray(env, LS, p);
    if (lua_pcall(LS, c, 1, 0))
    {
        const char *errmsg;
        if (lua_isstring(LS, -1))
            errmsg = lua_pushfstring(LS, "create %s error, msg: %s", name, lua_tostring(LS, -1));
        else
            errmsg = lua_pushfstring(LS, "create %s error, unknown msg", name);

        clearException(env);
        throwRuntimeError(env, errmsg);
        lua_pop(LS, 1);
        ReleaseChar(env, key, kstr);
        ReleaseChar(env, lcn, name);
        lua_pop(LS, 1);
        lua_unlock(LS);
        return NULL;
    }
    if (!lua_isuserdata(LS, -1))
    {
        throwRuntimeError(env, lua_pushfstring(LS, "create %s error, not a userdata!", name));
        ReleaseChar(env, key, kstr);
        ReleaseChar(env, lcn, name);
        lua_pop(LS, 1);
        lua_unlock(LS);
        return NULL;
    }
    UDjavaobject ud = (UDjavaobject)lua_touserdata(LS, -1);
    lua_setglobal(LS, kstr);
    ReleaseChar(env, key, kstr);
    ReleaseChar(env, lcn, name);
    lua_unlock(LS);
    return ud->jobj;
}
//// ----------------------------------------------------------------------------------------------------
//// -------------------------------------------jni end--------------------------------------------------
//// ----------------------------------------------------------------------------------------------------

/**
 * 注册ud
 * lcn: lua class name          nonnull
 * lpcn: lua parent class name  nullable
 * jcn: java class name         nonnull
 * jms: java methods            nullable
 * mc:  java methods count      >=0
 * lazy: 是否懒注册
 */
static void register_ud(JNIEnv *env, lua_State *L, const char *lcn, const char *lpcn, const char *jcn, char **jms, int mc, int lazy)
{
    jclass clz = (jclass)cj_get(jcn);
    if (!clz) {
        clz = (*env)->FindClass(env, jcn);
        if (!clz) {
            char *errorstr = joinstr("cannot find class ", jcn);
            throwRuntimeError(env, errorstr);
            m_malloc(errorstr, (strlen(errorstr) + 1) * sizeof(char), 0);
            return;
        }
        clz = GLOBAL(env, clz);
        cj_put(jcn, clz);
    }

    char * metaname = getUDMetaname(lcn);
    char * p_metaname = NULL;
    if (lpcn) {
        p_metaname = getUDMetaname(lpcn);
    }

    lua_lock(L);
    if (lazy) {
        push_lazy_init(L, clz, metaname, p_metaname, jms, mc);
    } else {
        push_init(env, L, clz, metaname, p_metaname, jms, mc);
        if (jms) free_methods(jms, mc);
    }
    lua_setglobal(L, lcn);
    lua_unlock(L);
    
    #if defined(J_API_INFO)
    if (p_metaname) m_malloc(p_metaname, (strlen(p_metaname) + 1) * sizeof(char), 0);
    m_malloc(metaname, (strlen(metaname) + 1) * sizeof(char), 0);
    #else
    if (p_metaname) free(p_metaname);
    free(metaname);
    #endif
}

/**
 * 生成jms，要和释放对应j_ms_gc
 */
static char ** get_methods_str(JNIEnv *env, jobjectArray ams, int methodCount, int methodStartIndex) {
    char ** jms = (char **) m_malloc(NULL, 0, sizeof(char *) * methodCount);
    memset(jms, 0, sizeof(char *) * methodCount);
    int j;
    size_t byte_size = sizeof(char);
    for(j = 0; j < methodCount; j++) {
        jstring m = (*env)->GetObjectArrayElement(env, ams, methodStartIndex + j);
        // jms[j] = GetString(env, m);

        const char * s = GetString(env, m);
        int len = (*env)->GetStringUTFLength(env, m) + 1;
        jms[j] = (char *) m_malloc(NULL, 0, byte_size * len);
        strcpy(jms[j], s);
        ReleaseChar(env, m, s);

        FREE(env, m);
    }
    return jms;
}
/**
 * 要和get_methods_str对应
 */
static inline void free_methods(char ** methods, int len) {
    #if defined(J_API_INFO)
    m_malloc(methods, sizeof(char*) * len, 0);
    #else
    free(methods);
    #endif
}
/**
 * 通过堆栈生成java对象，并push到栈顶
 * 若错误返回1
 * 正确返回0
 * return : -1: ud or errorstring
 */
static int new_java_obj(JNIEnv *env, lua_State *L, jclass clz, jmethodID con, const char *metaname, int offset) {
    int pc = lua_gettop(L) - offset;
    jobjectArray p = newLuaValueArrayFromStack(env, L, pc, offset);
    jobject javaObj = (*env)->NewObject(env, clz, con, (jlong)L, p);
    if ((*env)->ExceptionCheck(env))
    {
        ExceptionDescribe(env);
        clearException(env);
        FREE(env, javaObj);
        FREE(env, p);
        lua_pushstring(L, "exception throws in java methods!");
        return 1;
    }

    UDjavaobject ud = (UDjavaobject)lua_newuserdata(L, sizeof(javaUserdata));
    ud->jobj = GLOBAL(env, javaObj);
    ud->isStrongRef = IS_STRONG_REF(env, clz);
    ud->isSetKey = 0;

    //todo 是否做缓存，是否自己malloc ud->name = copystr(metaname);
    ud->name = lua_pushstring(L, metaname);
    lua_pop(L, 1);
    return 0;
}
//// ----------------------------------------------------------------------------------------------------
//// ---------------------------------------------lazy---------------------------------------------------
//// ----------------------------------------------------------------------------------------------------

#define CLZ_KEY_IN_TABLE  "__clz"
/**
 * 释放j_ms相关内存
 * 需要和get_methods_str对应
 */
static int j_ms_gc(lua_State *L) {
    lua_lock(L);
    __LID * ud = (__LID *) lua_touserdata(L, 1);
    lua_unlock(L);
    int i;
    for (i = 0; i < ud->len; i++) {
        char * m = ud->methods[i];
        if (m) m_malloc(m, sizeof(char) * (strlen(m) + 1), 0);
    }
    
    if (ud->methods)
        free_methods(ud->methods, ud->len);
    ud->methods = NULL;
    return 0;
}
/**
 * 对应execute_new_ud_lazy
 */
static void push_lazy_init(lua_State *L, jclass clz, const char *metaname, const char *p_metaname, char **methods, int len) {
    /// 若第一次创建metatable，创建__LID，且metaname[__LID]=__LID
    luaL_newmetatable(L, metaname);                             //metatable
    lua_pushstring(L, J_MS_METANAME);                           //__LID --metatable
    lua_pushvalue(L, -1);                                       //__LID --__LID-metatable
    lua_rawget(L, -3);                                          //metatable[__LID] --__LID-metatable
    __LID * lid;
    if (lua_isuserdata(L, -1)) {
        lid = lua_touserdata(L, -1);
        int oldLen = lid->len;
        lid->len += len;
        lid->methods = (char **) m_malloc(lid->methods, sizeof(char *) * oldLen, sizeof(char *) * lid->len);
        int i;
        for (i = oldLen; i < lid->len; i++) {
            lid->methods[i] = methods[i - oldLen];
        }
        lid->clz = clz;
        lid->p_meta = lua_pushstring(L, p_metaname);
        lua_pop(L, 1);
        m_malloc(methods, sizeof(char *) * len, 0);
        lua_pop(L, 2);                                          //metatable
    } else {
        lua_pop(L, 1);
        lid = (__LID *) lua_newuserdata(L, sizeof(__LID));      //lid --__LID-metatable
        lid->len = len;
        lid->methods = methods;
        lid->clz = clz;
        lid->p_meta = lua_pushstring(L, p_metaname);
        lua_pop(L, 1);
        if (luaL_newmetatable(L, J_MS_METANAME)) {              //l_metatable --lid-__LID-metatable
            SET_METATABLE(L);
            lua_pushstring(L, "__gc");
            lua_pushcfunction(L, j_ms_gc);
            lua_rawset(L, -3);
        }
        lua_setmetatable(L, -2);                                //lid --__LID-metatable
        lua_rawset(L, -3);                                      //metatable[__LID]=lid  metatable
    }
    //// metatable
    
    lua_pushstring(L, CLZ_KEY_IN_TABLE);                        //key --metatable
    UDjclass udj = (UDjclass) lua_newuserdata(L, sizeof(jclass));   //clz --key-metatable
    *udj = clz;
    lua_rawset(L, -3);                                          //metatable[__clz]=clz  metatable
    lua_pop(L, 1);
    
    lua_pushstring(L, metaname);                                //metaname
    lua_pushcclosure(L, execute_new_ud_lazy, 1);                //closure
}
/**
 * -1: lid -2: metatable
 * return class with -1: metatable
 */
static jclass init_lazy_metatable(JNIEnv *env, lua_State *L) {
    lua_lock(L);
    __LID *lid = (__LID *)lua_touserdata(L, -1);
    lua_pop(L, 1);
    jclass clz = lid->clz;
    fillUDMetatable(env, L, clz, lid->methods, lid->len, lid->p_meta);
    free_methods(lid->methods, lid->len);
    lid->methods = NULL;
    lid->len = 0;
    lua_pushstring(L, J_MS_METANAME);
    lua_pushnil(L);
    lua_rawset(L, -3);                  //metatable[__LID] = nil
    lua_unlock(L);
    return clz;
}
/**
 * 对应 push_lazy_init
 * upvalue顺序为: 1: metaname
 */
static int execute_new_ud_lazy(lua_State *L) {
    JNIEnv *env;
    int need = getEnv(&env);
    lua_lock(L);
    const char *metaname = lua_tostring(L, lua_upvalueindex(1));
    luaL_newmetatable(L, metaname);         //metatable
    lua_pushstring(L, J_MS_METANAME);       //__LID --metatable
    lua_rawget(L, -2);                      //metatable[__LID] --metatable

    jclass clz;
    /// 第一次设置的情况
    if (lua_isuserdata(L, -1)) {
        clz = init_lazy_metatable(env, L);      //metatable
    }
    /// 之后创建
    else {
        lua_pop(L, 1);                          //metatable
        lua_pushstring(L, CLZ_KEY_IN_TABLE);    //key --metatable
        lua_rawget(L, -2);                      //metatable[key] --metatable
        UDjclass udj = lua_touserdata(L, -1);
        clz = getuserdata(udj); 
        lua_pop(L, 1);                          //metatable
    }

    jmethodID con = findConstructor(env, clz, DEFAULT_CON_SIG);

    if (new_java_obj(env, L, clz, con, metaname, 1)) { //ud --metatable
        if (need) detachEnv();
        lua_unlock(L);
        lua_error(L);
        return 1;
    }

    lua_pushvalue(L, -2);       //metatable --ud-metatable
    lua_setmetatable(L, -2);    //ud --metatable
    lua_remove(L, -2);          //ud

    if (need) detachEnv();
    lua_unlock(L);
    return 1;
}
//// ----------------------------------------------------------------------------------------------------
//// ----------------------------------------------------------------------------------------------------
//// ----------------------------------------------------------------------------------------------------
//// ----------------------------------------------------------------------------------------------------
/**
 * 对应execute_new_ud
 */
static void push_init(JNIEnv *env, lua_State *L, jclass clz, const char *metaname, const char *p_metaname, char **methods, int len) {
    UDjclass udclz = (UDjclass)lua_newuserdata(L, sizeof(jclass));      // clz
    *udclz = clz;
    
    jmethodID cons = findConstructor(env, clz, DEFAULT_CON_SIG);
    UDjmethod udm = (UDjmethod)lua_newuserdata(L, sizeof(jmethodID));   // con,clz
    *udm = cons;

    luaL_newmetatable(L, metaname);                                     // table, con,clz
    fillUDMetatable(env, L, clz, methods, len, p_metaname);

    lua_pop(L, 1);                                                      // con,clz
    lua_pushstring(L, metaname);                                        // metaname,con,clz
    lua_pushcclosure(L, execute_new_ud, 3);
}

/**
 * 对应push_init
 * upvalue顺序为:
 *              1:UDjclass 
 *              2:constructor(UDjmethod)
 *              3:metaname(string)
 */
static int execute_new_ud(lua_State *L) {
    JNIEnv *env;
    int need = getEnv(&env);
    lua_lock(L);

    /// 第1个参数
    int idx = lua_upvalueindex(1);
    jclass clz = getuserdata((UDjclass)lua_touserdata(L, idx));

    /// 第2个参数
    idx = lua_upvalueindex(2);
    jmethodID con = getuserdata((UDjmethod)lua_touserdata(L, idx));

    /// 第3个参数
    idx = lua_upvalueindex(3);
    const char *metaname = lua_tostring(L, idx);

    if (new_java_obj(env, L, clz, con, metaname, 0)) {
        if (need) detachEnv();
        lua_unlock(L);
        lua_error(L);
        return 1;
    }

    luaL_getmetatable(L, metaname);
    lua_setmetatable(L, -2);

    if (need) detachEnv();
    lua_unlock(L);
    return 1;
}
/**
 * -1: metatable
 * return void with -1: metatable
 */
static void fillUDMetatable(JNIEnv *env, lua_State *LS, jclass clz, char **jms, int len, const char *parent_mn) {
    SET_METATABLE(LS);
    if (parent_mn) {
        luaL_getmetatable(LS, parent_mn);
        if (lua_istable(LS, -1)) {                  // metatable
            lua_pushstring(LS, J_MS_METANAME);      //__LID --metatable
            lua_rawget(LS, -2);                     //metatable[__LID] --metatable
            if (lua_isuserdata(LS, -1)) {
                __LID *lid = (__LID *)lua_touserdata(LS, -1);
                if (lid->methods != jms) init_lazy_metatable(env, LS);       //metatable
                else lua_pop(LS, 1);                //metatable
            } else {
                lua_pop(LS, 1);                     //metatable
            }
            /// 如果是相同的table，及parent的名称和自己相同，则不用拷贝
            if (!lua_rawequal(LS, -1, -2))
                copyTable(LS, -1, -2);
        }
        lua_pop(LS, 1);
    }
    jmethodID method;
    int i;
    char *m;
    for (i = 0; i < len; i++) {
        m = jms[i];
        method = (*env)->GetMethodID(env, clz, m, DEFAULT_SIG);
        lua_pushstring(LS, m);
        pushMethodClosure(LS, method, m);
        lua_rawset(LS, -3); //metatable.m = closure
        #if defined(J_API_INFO)
        m_malloc(m, (strlen(m) + 1) * sizeof(char), 0);
        #else
        free(m);
        #endif
        jms[i] = NULL;
    }
    /// 设置gc方法
    pushUserdataGcClosure(env, LS, clz);
    /// 设置需要返回bool的方法，比如__eq
    pushUserdataBoolClosure(env, LS, clz);
    /// 设置__tostring
    pushUserdataTostringClosure(env, LS, clz);
}


//// ----------------------------------------------------------------------------------------------------
//// ----------------------------------------------------------------------------------------------------
//// ----------------------------------------------------------------------------------------------------
//// ----------------------------------------------------------------------------------------------------
/**
 * 对应userdata_tostring_fun
 */
static void pushUserdataTostringClosure(JNIEnv *env, lua_State *L, jclass clz) {
    jmethodID m = (*env)->GetMethodID(env, clz, JAVA_TOSTRING, "()" STRING_CLASS);
    if ((*env)->ExceptionCheck(env)) {
        clearException(env);
        return;
    }
    lua_pushstring(L, "__tostring");

    UDjmethod udm = (UDjmethod)lua_newuserdata(L, sizeof(jmethodID));
    *udm = m;

    lua_pushcclosure(L, userdata_tostring_fun, 1);

    lua_rawset(L, -3);
}
/**
 * 对应pushUserdataTostringClosure
 * upvalue顺序为:
 *              1:UDjmethod
 */
static int userdata_tostring_fun(lua_State *L) {
    lua_lock(L);
    if (!lua_isuserdata(L, 1))
    {
        lua_pushstring(L, "use ':' instead of '.' to call method!!");
        lua_unlock(L);
        lua_error(L);
        return 1;
    }

    JNIEnv *env;
    int need = getEnv(&env);
    
    jmethodID m = getuserdata((UDjmethod)lua_touserdata(L, lua_upvalueindex(1)));

    UDjavaobject ud = (UDjavaobject)lua_touserdata(L, 1);

    jstring r = (*env)->CallObjectMethod(env, ud->jobj, m);
    clearException(env);
    const char *str = GetString(env, r);
    if (str) {
        lua_pushstring(L, str);
        ReleaseChar(env, r, str);
        FREE(env, r);
        if (need) detachEnv();
        lua_unlock(L);
        return 1;
    }
    lua_pushstring(L, "call tostring exception");
    if (need) detachEnv();
    lua_unlock(L);
    return 1;
}
/**
 * 对应userdata_bool_fun
 */
static void pushUserdataBoolClosure(JNIEnv *env, lua_State *L, jclass clz) {
    static const char *const luaT_eventname[] = {
        "__eq",
        // "__lt",
        // "__le",
    };
    static const char *const java_methodname[] = {
        "__onLuaEq",
        // "__onLuaLt",
        // "__onLuaLe",
    };
    static int count = 1;

    int i;
    jmethodID m;
    UDjmethod udm;
    for (i = 0; i < count; i++)
    {
        m = (*env)->GetMethodID(env, clz, java_methodname[i], "(" OBJECT_CLASS ")Z");
        if ((*env)->ExceptionCheck(env)) {
            clearException(env);
            continue;
        }

        lua_pushstring(L, luaT_eventname[i]);
        
        udm = (UDjmethod)lua_newuserdata(L, sizeof(jmethodID));
        *udm = m;
        lua_pushcclosure(L, userdata_bool_fun, 1);
        lua_rawset(L, -3);
    }
}
/**
 * 对应pushUserdataBoolClosure
 * upvalue顺序为:
 *              1:UDjmethod
 */
static int userdata_bool_fun(lua_State *L) {
    lua_lock(L);
    if (!lua_isuserdata(L, 1) || !lua_isuserdata(L, 2)) {
        lua_pushboolean(L, 0);
        lua_unlock(L);
        return 1;
    }
    UDjavaobject ud = (UDjavaobject)lua_touserdata(L, 1);
    UDjavaobject other = (UDjavaobject)lua_touserdata(L, 2);
    if (ud == other) {
        lua_pushboolean(L, 1);
        lua_unlock(L);
        return 1;
    } else if (!isJavaUserdata(other)) {
        lua_pushboolean(L, 0);
        lua_unlock(L);
        return 1;
    }

    JNIEnv *env;
    int need = getEnv(&env);

    jmethodID m = getuserdata((UDjmethod)lua_touserdata(L, lua_upvalueindex(1)));

    jboolean r = (*env)->CallBooleanMethod(env, ud->jobj, m, other->jobj);
    clearException(env);
    lua_pushboolean(L, r);

    if (need) detachEnv();
    lua_unlock(L);
    return 1;
}
/**
 * 对应gc_userdata
 */
static void pushUserdataGcClosure(JNIEnv *env, lua_State *L, jclass clz) {
    lua_pushstring(L, "__gc");

    jmethodID gc = (*env)->GetMethodID(env, clz, JAVA_GC_METHOD, "()V");
    if ((*env)->ExceptionCheck(env)) {
        gc = NULL;
        clearException(env);
    }

    if (gc) {
        UDjmethod udm = (UDjmethod)lua_newuserdata(L, sizeof(jmethodID));
        *udm = gc;
    } else {
        lua_pushnil(L);
    }

    lua_pushcclosure(L, gc_userdata, 1);

    lua_rawset(L, -3);
}
/**
 * 对应pushUserdataGcClosure
 * upvalue顺序为:
 *              1:UDjmethod gcmethod nilable
 */
static int gc_userdata(lua_State *L) {
    lua_lock(L);
    if (!lua_isuserdata(L, 1)) {
        lua_pushstring(L, "use ':' instead of '.' to call method!!");
        lua_unlock(L);
        lua_error(L);
        return 0;
    }
    JNIEnv *env;
    int need = getEnv(&env);

    UDjavaobject ud = (UDjavaobject)lua_touserdata(L, 1);
    int idx = lua_upvalueindex(1);
    jmethodID gcm = NULL;
    if (lua_isuserdata(L, idx)) {
        gcm = getuserdata((UDjmethod)lua_touserdata(L, idx));
    }
    lua_unlock(L);

    if (gcm)
        (*env)->CallVoidMethod(env, ud->jobj, gcm);
    if ((*env)->ExceptionCheck(env)){
        ExceptionDescribe(env);
        clearException(env);
    }
    UNGLOBAL(env, ud->jobj);
    if (need) detachEnv();
    // todo 考虑是否需要free，是否做缓存 free(ud->name);
    return 0;
}
/**
 * 对应executeJavaUDFunction
 */
static void pushMethodClosure(lua_State *L, jmethodID m, const char *mn) {
    UDjmethod udm = (UDjmethod)lua_newuserdata(L, sizeof(jmethodID));
    *udm = m;

    lua_pushstring(L, mn);

    lua_pushcclosure(L, executeJavaUDFunction, 2);
}
/**
 * 对应pushMethodClosure
 * upvalue顺序为:
 *              1:UDjmethod
 *              2:string methodname
 */
static int executeJavaUDFunction(lua_State *L) {
    lua_lock(L);
    if (!lua_isuserdata(L, 1)) {
        lua_pushstring(L, "use ':' instead of '.' to call method!!");
        lua_unlock(L);
        lua_error(L);
        return 1;
    }

    JNIEnv *env;
    int need = getEnv(&env);

    /// 第1个参数为方法
    int idx = lua_upvalueindex(1);
    UDjmethod udm = (UDjmethod)lua_touserdata(L, idx);
    jmethodID m = getuserdata(udm);

    /// 第2个参数为方法名称
    idx = lua_upvalueindex(2);
    const char *n = lua_tostring(L, idx);

    int pc = lua_gettop(L) - 1; //去掉底部userdata
    UDjavaobject ud = (UDjavaobject)lua_touserdata(L, 1);

    jobjectArray p = newLuaValueArrayFromStack(env, L, pc, 2);
    jobjectArray r = (*env)->CallObjectMethod(env, ud->jobj, m, p);
    if ((*env)->ExceptionCheck(env))
    {
        ExceptionDescribe(env);
        clearException(env);
        FREE(env, p);
        FREE(env, r);
        lua_pushstring(L, "exception throws in java methods!");
        if (need) detachEnv();
        lua_unlock(L);
        lua_error(L);
        return 1;
    }
    FREE(env, p);
    if (!r) {
        lua_settop(L, 1);
        if (need) detachEnv();
        lua_unlock(L);
        return 1;
    }
    luaC_checkGC(L);
    int ret = pushJavaArray(env, L, r);
    FREE(env, r);
    if (need) detachEnv();
    lua_unlock(L);
    return ret;
}