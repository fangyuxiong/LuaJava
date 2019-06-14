//
// Created by Xiong.Fangyu 2019/03/13.
//

#ifndef J_INFO_H
#define J_INFO_H

#include "global_define.h"

#define JAVA_CONSTRUCTOR "<init>"
#define JAVA_VALUE_OF "valueOf"

#define findTypeClass(env, type) (*env)->FindClass(env, JAVA_PATH "" type)
#define findDefaultConstructor(env, type) (*env)->GetMethodID(env, type, JAVA_CONSTRUCTOR, "()V")
#define findConstructor(env, type, sig) (*env)->GetMethodID(env, type, JAVA_CONSTRUCTOR, "(" sig ")V")

#define GetArrLen(env, arr) (int)((*env)->GetArrayLength(env, arr))

void initJavaInfo(JNIEnv *);
jobject newLuaNumber(JNIEnv *, jdouble);
jobject newLuaString(JNIEnv *, const char *);
jobject newLuaTable(JNIEnv *, lua_State *, int);
jobject newLuaFunction(JNIEnv *, lua_State *, int);
jobject newLuaUserdata(JNIEnv *, lua_State *, int, UDjavaobject);
jobject newLuaThread(JNIEnv *env, lua_State *L, int idx);
/**
 * 把java的userdata对象push到栈上
 */
void pushUserdataFromJUD(JNIEnv *env, lua_State *L, jobject obj);
/**
 * 将栈中idx位置的数据转为java中的LuaValue
 * idx可为正数或负数
 */
jobject toJavaValue(JNIEnv *env, lua_State *L, int idx);
/**
 * 调用函数时使用，将栈中所有的数据读成LuaValue，并保存为对象
 */
jobjectArray newLuaValueArrayFromStack(JNIEnv *env, lua_State *L, int count, int stackoffset);
/**
 * 将LuaValue对象obj转成lua数据类型，并push到栈顶
 */
void pushJavaValue(JNIEnv *env, lua_State *L, jobject obj);
/**
 * 函数返回时使用，将java方法结果转成lua数据，push到栈中，并返回参数个数
 */
int pushJavaArray(JNIEnv *env, lua_State *L, jobjectArray arr);
/**
 * 抛出调用异常
 */
void throwInvokeError(JNIEnv *env, const char *errmsg);
/**
 * 超出运行时异常
 */
void throwRuntimeError(JNIEnv *env, const char *msg);
/**
 * lua调用gc时调用
 */
void callbackLuaGC(JNIEnv *env, lua_State *L);
/**
 * 获取java环境
 * 若需要自行调用detach,返回1,否则返回0
 */
int getEnv(JNIEnv **out);
/**
 * detach
 */
void detachEnv();
#endif //J_INFO_H