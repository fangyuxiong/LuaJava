/**
 * Created by Xiong.Fangyu 2019/03/13
 */

#include <string.h>
#include "cache.h"
#include "lobject.h"
#include "mlog.h"
#include "debug_info.h"
#include "map.h"
#include "m_mem.h"
#include "llimits.h"

/**
 * 从GNV表中移除native数据
 */
static void removeValueFromGNV(JNIEnv *env, lua_State *L, const char *key, int ltype);

#if defined(SAVE_LUAVALUE)
/**
 * 从LVR表中移除LuaValue数据
 */
static void removeLuaValue(JNIEnv *env, lua_State *L, const char *key, int type);
#endif  //SAVE_LUAVALUE

void jni_removeNativeValue(JNIEnv *env, jobject job, jlong L, jstring key, jint lt)
{
    const char *k = GetString(env, key);
    if (k) {
        lua_lock((lua_State *)L);
        removeValueFromGNV(env, (lua_State *)L, k, lt);
#if defined(SAVE_LUAVALUE)
        removeLuaValue(env, (lua_State *)L, k, lt);
#endif  //SAVE_LUAVALUE
        lua_unlock((lua_State *)L);
        ReleaseChar(env, key, k);
    }
}

static void init_map();

void init_cache(lua_State *L)
{
    init_map();
    lua_lock(L);
    /// 创建GNV表 需要针对4种类型
    lua_createtable(L, 4, 0);
    lua_setglobal(L, GNV);
#if defined(SAVE_LUAVALUE)
    /// 创建LVR表 针对2中类型即可 table, function
    lua_createtable(L, 2, 0);
    lua_setglobal(L, LVR);
#endif  //SAVE_LUAVALUE
    lua_unlock(L);
}

#define GNV_VIDX 1
#define GNV_NIDX 2

/**
 * 从GNV 表中，找到对应类型的表
 * 主要类型为 table, function, userdata, thread
 */
static void getGlobalNVTableForType(lua_State *L, int type)
{
    lua_lock(L);
    lua_getglobal(L, GNV);                  // -1: GNV
    lua_rawgeti(L, -1, (lua_Integer)type);  // -1: value --GNV
    if (lua_istable(L, -1)) {
        lua_remove(L, -2);
        lua_unlock(L);
        return;
    }
    lua_pop(L, 1);                          // -1: GNV
    lua_createtable(L, 0, 10);              // -1: table --GNV
    lua_pushvalue(L, -1);                   // -1: table --table-GNV
    lua_rawseti(L, -3, (lua_Integer)type);  // -1: table -GNV
    lua_remove(L, -2);
    lua_unlock(L);
}

const char *copyValueToGNV(lua_State *L, int idx)
{
    lua_lock(L);
    lua_pushvalue(L, idx);                            // -1: value
    const void *addr = lua_topointer(L, -1);
    int type = lua_type(L, -1);
    getGlobalNVTableForType(L, type);                 // -1: table --value
    const char *key = lua_pushfstring(L, "%p", addr); //-1: key --table-value
    lua_rawget(L, -2);                                // -1: newtable; --table-value
    if (lua_istable(L, -1)) {
        lua_rawgeti(L, -1, GNV_NIDX);       //-1: num(引用计数) --newtable-table-value
        int num = lua_tointeger(L, -1);
        lua_pushinteger(L, num + 1);        // -1: num(增加计数) --num(原计数)-newtable-table-value
        lua_rawseti(L, -3, GNV_NIDX);       // newtable[2] = num(增加计数) -1:num(原计数) --newtable-table-value
        lua_pop(L, 4);
        lua_unlock(L);
        return key;
    }
    lua_pop(L, 1);                  // -1: table --value
    lua_pushstring(L, key);         // -1: key --table-value
    /// 增加引用计数，创建一个数组table，1指向value，2指向引用计数
    lua_createtable(L, 2, 0);       // -1: newtable --key-table-value
    lua_pushinteger(L, 1);          // -1: 1 --newtable-key-table-value
    lua_rawseti(L, -2, GNV_NIDX);   //设置计数 newtable[2] = 1  -1：newtable --key-table-value
    lua_pushvalue(L, -4);           // -1: value --newtable-key-table-value
    lua_rawseti(L, -2, GNV_VIDX);   //newtable[1] = value   -1: newtable --key-table-value
    lua_rawset(L, -3);              // table[key] = newtable  -1: table --value
    lua_pop(L, 2);
    lua_unlock(L);
    return key;
}

void getValueFromGNV(lua_State *L, const char *key, int ltype)
{
    lua_lock(L);
    if (!key) {
        lua_pushnil(L);
        lua_unlock(L);
        return;
    }
    
    getGlobalNVTableForType(L, ltype);
    lua_pushstring(L, key); //-1:key --table
    lua_rawget(L, -2);      //-1:newtable --table
    lua_remove(L, -2);      //-1:newtable
    if (lua_isnil(L, -1)) {
        lua_unlock(L);
        return;
    }
    lua_rawgeti(L, -1, GNV_VIDX);  //-1:newtable[1] --newtable
    lua_remove(L, -2);      //-1:newtable[1]
    lua_unlock(L);
}

static void removeValueFromGNV(JNIEnv *env, lua_State *L, const char *key, int ltype)
{
    if (!key) {
        return;
    }
    lua_lock(L);
    getGlobalNVTableForType(L, ltype);  //-1: table
    lua_pushstring(L, key);             //-1: key--table
    lua_rawget(L, -2);                  //-1: newtable --table
    if (lua_isnil(L, -1)) {
        lua_pop(L, 2);
        lua_unlock(L);
        return;
    }
    lua_rawgeti(L, -1, GNV_NIDX);       //newtable[2]  -1: num(引用计数) --newtable-table
    int num = lua_tointeger(L, -1) - 1; //减去一个计数
    if (num > 0) {                      //还有计数的情况下，替换计数
        lua_pushinteger(L, num);        //-1: num --num(原计数)-newtable-table
        lua_rawseti(L, -3, GNV_NIDX);   //newtable[2] = num  -1:num(原计数) --newtable-table
        lua_pop(L, 3);
        lua_unlock(L);
        return;
    }
    LOGE("引用计数为0，删除，类型为:%d",ltype);
    /// 引用计数为0的情况
    lua_pop(L, 1);                  //-1: newtable --table
    lua_pushstring(L, key);         //-1: key --newtable-table
    lua_rawgeti(L, -2, GNV_VIDX);   //newtable[1] -1: value --key-newtable-table
    lua_remove(L, -3);              //-1: value --key-table
    if (lua_isnil(L, -1)) {
        lua_rawset(L, -3);          //table[key] = nil   -1: table
        lua_pop(L, 1);
        lua_unlock(L);
        return;
    }
    if (ltype == LUA_TUSERDATA) {
        UDjavaobject ud = (UDjavaobject)lua_touserdata(L, -1);
        ud->isSetKey = 0;
    }
    lua_pop(L, 1);                  //-1: key --table
    lua_pushnil(L);
    lua_rawset(L, -3);              //table[key] = nil    -1: table
    lua_pop(L, 1);
    lua_unlock(L);
}

///---------------------------------------------------------------------------
///------------------------classname->jclass----------------------------------
///---------------------------------------------------------------------------
/// 存储java class 名称 --> 对应global jclass
static Map * __map = NULL;

static void s_free(void * p) {
    #if defined(J_API_INFO)
        m_malloc(p, (strlen(p) + 1) * sizeof(char), 0);
    #else
        free(p);
    #endif
}

static int str_equals(const void * a, const void * b) {
    return strcmp((const char *)a, (const char *)b) == 0;
}

#if defined(J_API_INFO)
static size_t str_size(void * k) {
    char * str = (char *) k;
    return sizeof(char) * (strlen(str) + 1);
}

static size_t obj_size(void * v) {
    return 0;//sizeof(jobject);
}
#endif

unsigned int str_hash (const void * k) {
    const char * str = (const char *)k;
    size_t l = strlen(str);
    unsigned int h = (unsigned int)l ^ 891604371;
    size_t l1;
    size_t step = (l >> 5) + 1;
    for (l1 = l; l1 >= step; l1 -= step)
        h = h ^ ((h<<5) + (h>>2) + (char)str[l1 - 1]);
    return h;
}

static void init_map() {
    if (!__map) {
        __map = map_new(NULL, 50);
        if (map_ero(__map)) {
            map_free(__map);
            __map = NULL;
        } else {
            map_set_free(__map, s_free, NULL);
            map_set_equals(__map, str_equals);
            map_set_hash(__map, str_hash);
            #if defined(J_API_INFO)
            map_set_sizeof(__map, NULL, NULL);
            #endif
        }
    }
}
/**
 * 存储类名对应的jclass(global变量)
 */
void cj_put(const char * name, void* obj) {
    init_map();
    if (!__map) {
        LOGE("cj_put-- map is not init!!!");
        return;
    }
    int len = strlen(name)+1;
    char * copy = (char *) m_malloc(NULL, 0, sizeof(char) * len);
    strcpy(copy, name);
    void * v = map_put(__map, copy, obj);
    if (v) m_malloc(copy, sizeof(char) * len, 0);
    #if defined(J_API_INFO)
    if (!v) remove_by_pointer(copy, sizeof(char) * len);
    // LOGE("value for %s is %p, value: %p", copy, v, obj);
    #endif
}
/**
 * 取出类名name 对应的jclass(global变量)
 */
void* cj_get(const char * name) {
    if (!__map) {
        LOGE("cj_get-- map is not init!!!");
        return NULL;
    }
    return map_get(__map, name);
}
#if defined(J_API_INFO)
/**
 * 打印map中内容
 */
void cj_log() {
    if (!__map) {
        LOGE("cj_log-- map is not init!!!");
        return;
    }
    size_t len = map_size(__map);
    if (len <= 0) {
        LOGI("map has no value");
        return;
    }
    LOGI("map has %d values, map table has %d size.", (int)len, (int)map_table_size(__map));
}

/**
 * 获取map消耗的内存
 */
size_t cj_mem_size() {
    return __map ? map_mem(__map) : 0;
}
#endif  //J_API_INFO
#if defined(SAVE_LUAVALUE)
/**
 * 从GNV 表中，找到对应类型的表
 * 主要类型为 table, function, userdata, thread
 */
static void getLVRTableForType(lua_State *L, int type)
{
    lua_lock(L);
    lua_getglobal(L, LVR);                  // -1: LVR
    lua_rawgeti(L, -1, (lua_Integer)type);  // -1: value --LVR
    if (lua_istable(L, -1)) {
        lua_remove(L, -2);
        lua_unlock(L);
        return;
    }
    lua_pop(L, 1);                          // -1: LVR
    lua_createtable(L, 0, 10);              // -1: table --LVR
    lua_pushvalue(L, -1);                   // -1: table --table-LVR
    lua_rawseti(L, -3, (lua_Integer)type);  // -1: table -LVR
    lua_remove(L, -2);
    lua_unlock(L);
}

/**
 * 保存LuaValue对象，将会把ref变为global变量
 */
void saveLuaValue(JNIEnv *env, lua_State *L, const char *key, int type, jobject ref)
{
    if (!key)
        return;
    lua_lock(L);
    getLVRTableForType(L, type);                        // -1: LVR
    lua_pushstring(L, key);                             // -1: key --LVR
    jobject *ud = (jobject *)lua_newuserdata(L, sizeof(jobject));  // -1: ud --key-LVR
    *ud = GLOBAL(env, ref);
    lua_rawset(L, -3);
    lua_pop(L, 1);
    lua_unlock(L);
}
/**
 * 从LVR表中获取相关LuaValue对象
 */
jobject getLuaValue(lua_State *L, const char *key, int type)
{
    if (!key)
        return;
    lua_lock(L);
    getLVRTableForType(L, type);    // -1: LVR
    lua_pushstring(L, key);         // -1: key --LVR
    lua_rawget(L, -2);              // -1: value --LVR
    if (lua_isuserdata(L, -1))
    {
        jobject *ud = (jobject *)lua_touserdata(L, -1);
        lua_pop(L, 2);
        lua_unlock(L);
        return *ud;
    }
    lua_pop(L, 2);
    lua_unlock(L);
    return NULL;
}
/**
 * 把相关对象从表中释放掉
 */
static inline void removeLuaValue(JNIEnv *env, lua_State *L, const char *key, int type)
{
    if (!key)
        return;
    lua_lock(L);
    getLVRTableForType(L, type);    // -1: LVR
    lua_pushstring(L, key);         // -1: key --LVR
    lua_rawget(L, -2);              // -1: value --LVR
    if (lua_isnil(L, -1))
    {
        lua_pop(L, 2);
        lua_unlock(L);
        return;
    }
    jobject *ud = (jobject *)lua_touserdata(L, -1);
    UNGLOBAL(env, *ud);
    lua_pop(L, 1);
    lua_pushstring(L, key);
    lua_pushnil(L);
    lua_rawset(L, -3);
    lua_pop(L, 1);
    lua_unlock(L);
}
#endif  //SAVE_LUAVALUE