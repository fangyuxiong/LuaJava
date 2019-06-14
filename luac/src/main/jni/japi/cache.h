/**
 * Created by Xiong.Fangyu 2019/03/13
 */

#ifndef L_CACHE_H
#define L_CACHE_H

#include "global_define.h"

/// GNV表，存放lua变量，防止被lua虚拟机回收
#define GNV "___Global_Native_Value"

#if defined(SAVE_LUAVALUE)
/// LVR表，存放java的LuaValue类型，减少对象创建次数，可不用
#define LVR "___LUA_VALUE_REF"
#endif  //SAVE_LUAVALUE
/**
 * 初始化表
 */
void init_cache(lua_State *L);
///---------------------------------------------------------------------------
///-------------------------------GNV-----------------------------------------
///---------------------------------------------------------------------------
/**
 * 获取保存在GNV表中的数据
 * idx: 存储的位置 see copyValueToGNV
 * ltype: lua类型 Table, Function, Userdata, Thread
 */
void getValueFromGNV(lua_State *L, const char *key, int ltype);
/**
 * 将idx位置的数据(Table, Function, Userdata, Thread)保存到GNV 表中
 * 返回表中位置
 */
const char *copyValueToGNV(lua_State *L, int idx);

///---------------------------------------------------------------------------
///------------------------classname->jclass----------------------------------
///---------------------------------------------------------------------------
/**
 * 存储类名对应的jclass(global变量)
 */
void cj_put(const char * name, void* obj);
/**
 * 取出类名name 对应的jclass(global变量)
 */
void* cj_get(const char * name);
#if defined(J_API_INFO)
/**
 * 打印map中内容
 */
void cj_log();
/**
 * 获取map消耗的内存
 */
size_t cj_mem_size();
#endif  //J_API_INFO
#if defined(SAVE_LUAVALUE)
///---------------------------------------------------------------------------
///-------------------------------LVR-----------------------------------------
///---------------------------------------------------------------------------
/**
 * 保存LuaValue对象，将会把ref变为global变量
 */
void saveLuaValue(JNIEnv *env, lua_State *L, const char *key, int type, jobject ref);
/**
 * 从LVR表中获取相关LuaValue对象
 */
jobject getLuaValue(lua_State *L, const char *key, int type);
#endif  //SAVE_LUAVALUE
#endif //L_CACHE_H