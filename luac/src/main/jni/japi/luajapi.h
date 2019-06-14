/**
 * Created by Xiong.Fangyu 2019/02/22
 */

#ifndef LUA_J_API_H
#define LUA_J_API_H

#include "global_define.h"
#include "jlog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lauxlib.h"
#include "lualib.h"
#include "lobject.h"
#include "lstate.h"
#include "mlog.h"
#include "jinfo.h"
#include "cache.h"

// ------------------------------------------------------------------------------
// -------------------------------JNI METHOD ------------------------------------
// ------------------------------------------------------------------------------

jboolean jni_check32bit(JNIEnv *env, jobject jobj);
jboolean jni_isSAESFile(JNIEnv *env, jobject jobj, jstring path);
void jni_openSAES(JNIEnv *env, jobject jobj, jboolean open);
jlong jni_lvmMemUse(JNIEnv * env, jobject jobj, jlong L);
jlong jni_allLvmMemUse(JNIEnv *env, jobject jobj);
jlong jni_globalObjectSize(JNIEnv *env, jobject jobj);
void jni_logMemoryInfo(JNIEnv *env, jobject jobj);
void jni_setGcOffset(JNIEnv *env, jobject jobj, int offset);
// --------------------------compile --------------------------
jint jni_compileAndSave(JNIEnv *env, jobject jobj, jlong L, jstring fn, jstring chunkname, jbyteArray data);
jint jni_compilePathAndSave(JNIEnv *env, jobject jobj, jlong L, jstring fn, jstring src, jstring chunkname);
jint jni_savePreloadData(JNIEnv *env, jobject jobj, jlong LS, jstring savePath, jstring chunkname);
jint jni_saveChunk(JNIEnv *env, jobject jobj, jlong LS, jstring path, jstring chunkname);
// --------------------------L State--------------------------
jlong jni_createLState(JNIEnv *env, jobject jobj, jboolean debug);
void jni_setBasePath(JNIEnv *env, jobject jobj, jlong LS, jstring path, jboolean autosave);
jint jni_registerIndex(JNIEnv *env, jobject jobj);
void jni_close(JNIEnv *env, jobject jobj, jlong L_state_pointer);
jobjectArray jni_dumpStack(JNIEnv *env, jobject jobj, jlong L);
void jni_removeStack(JNIEnv *env, jobject jobj, jlong L, jint idx);
void jni_pop(JNIEnv *env, jobject jobj, jlong L, jint c);
jint jni_getTop(JNIEnv *env, jobject jobj, jlong L);
void jni_removeNativeValue(JNIEnv *env, jobject job, jlong L, jstring k, jint lt);
// --------------------------load execute--------------------------
jint jni_loadData(JNIEnv *env, jobject jobj, jlong L_state_pointer, jstring name, jbyteArray data);
jint jni_loadFile(JNIEnv *env, jobject jobj, jlong L_state_pointer, jstring path, jstring chunkname);
jint jni_doLoadedData(JNIEnv *env, jobject jobj, jlong L_state_pointer);
jint jni_startDebug(JNIEnv *env, jobject jobj, jlong LS, jbyteArray data, jstring ip, jint port);
jboolean jni_setMainEntryFromPreload(JNIEnv *env, jobject jobj, jlong L, jstring name);
void jni_preloadData(JNIEnv *env, jobject jobj, jlong L, jstring name, jbyteArray data);
void jni_preloadFile(JNIEnv *env, jobject jobj, jlong L, jstring name, jstring path);
// --------------------------table--------------------------
jstring jni_createTable(JNIEnv *env, jobject jobj, jlong L);

jint jni_getTableSize(JNIEnv *env, jobject jobj, jlong L, jstring table);

void jni_setTableNumber(JNIEnv *env, jobject jobj, jlong L, jstring table, jint k, jdouble v);
void jni_setTableBoolean(JNIEnv *env, jobject jobj, jlong L, jstring table, jint k, jboolean v);
void jni_setTableString(JNIEnv *env, jobject jobj, jlong L, jstring table, jint k, jstring v);
void jni_setTableNil(JNIEnv *env, jobject jobj, jlong L, jstring table, jint k);
void jni_setTableChild(JNIEnv *env, jobject jobj, jlong L, jstring table, jint k, jobject c, jint lt);

void jni_setTableSNumber(JNIEnv *env, jobject jobj, jlong L, jstring table, jstring k, jdouble v);
void jni_setTableSBoolean(JNIEnv *env, jobject jobj, jlong L, jstring table, jstring k, jboolean v);
void jni_setTableSString(JNIEnv *env, jobject jobj, jlong L, jstring table, jstring k, jstring v);
void jni_setTableSNil(JNIEnv *env, jobject jobj, jlong L, jstring table, jstring k);
void jni_setTableSChild(JNIEnv *env, jobject jobj, jlong L, jstring table, jstring k, jobject c, jint lt);

jobject jni_getTableValue(JNIEnv *env, jobject jobj, jlong L, jstring table, jint k);
jobject jni_getTableSValue(JNIEnv *env, jobject jobj, jlong L, jstring table, jstring k);

jobject jni_createUserdataAndSet(JNIEnv *env, jobject jobj, jlong L, jstring key, jstring lcn, jobjectArray p);

jobject jni_getTableEntry(JNIEnv *env, jobject jobj, jlong L, jstring table);

jboolean jni_startTraverseTable(JNIEnv *env, jobject jobj, jlong L, jstring table);
jobjectArray jni_nextEntry(JNIEnv *env, jobject jobj, jlong L, jboolean isGlobal);
void jni_endTraverseTable(JNIEnv *env, jobject jobj, jlong L);
// --------------------------function--------------------------
jobjectArray jni_invoke(JNIEnv *env, jobject jobj, jlong L, jstring function, jobjectArray params, jint rc);

void jni_registerStaticClassSimple(JNIEnv *env, jobject jobj, jlong L, jstring jn, jstring ln, jstring lpcn, jobjectArray methods);

void jni_registerStaticClass(JNIEnv *env, jobject jobj, jlong L, jstring jn, jstring ln, jobjectArray methods, jobjectArray lmn, jintArray pcs);

void jni_registerUserdata(JNIEnv *env, jobject jobj, jlong L, jstring lcn, jstring lpcn, jstring jcn, jobjectArray jms);
void jni_registerAllUserdata(JNIEnv *env, jobject jobj, jlong L, jobjectArray lcns, jobjectArray lpcns, jobjectArray jcns, jintArray mc, jbooleanArray lazy, jobjectArray jms);

void jni_registerUserdataLazy(JNIEnv *env, jobject jobj, jlong L, jstring lcn, jstring lpcn, jstring jcn, jobjectArray jms);

void jni_registerNumberEnum(JNIEnv *env, jobject jobj, jlong L, jstring lcn, jobjectArray keys, jdoubleArray values);
void jni_registerStringEnum(JNIEnv *env, jobject jobj, jlong L, jstring lcn, jobjectArray keys, jobjectArray values);
// --------------------------end--------------------------

static JNINativeMethod jni_methods[] = {
    {"_check32bit", "()Z", (void *)jni_check32bit},
    {"_isSAESFile", "(" STRING_CLASS ")Z", (void *)jni_isSAESFile},
    {"_openSAES", "(Z)V", (void *)jni_openSAES},
    {"_lvmMemUse", "(J)J", (void *)jni_lvmMemUse},
    {"_allLvmMemUse", "()J", (void *)jni_allLvmMemUse},
    {"_globalObjectSize", "()J", (void *)jni_globalObjectSize},
    {"_logMemoryInfo", "()V", (void *)jni_logMemoryInfo},
    {"_setGcOffset", "(I)V", (void *)jni_setGcOffset},

    {"_compileAndSave", "(J" STRING_CLASS "" STRING_CLASS "[B)I", (void *)jni_compileAndSave},
    {"_compilePathAndSave", "(J" STRING_CLASS "" STRING_CLASS "" STRING_CLASS ")I", (void *)jni_compilePathAndSave},
    {"_savePreloadData", "(J" STRING_CLASS "" STRING_CLASS ")I", (void *)jni_savePreloadData},
    {"_saveChunk", "(J" STRING_CLASS "" STRING_CLASS ")I", (void *)jni_saveChunk},
    
    {"_createLState", "(Z)J", (void *)jni_createLState},
    {"_setBasePath", "(J" STRING_CLASS "Z)V", (void *)jni_setBasePath},
    {"_registerIndex", "()I", (void *)jni_registerIndex},
    {"_close", "(J)V", (void *)jni_close},
    {"_dumpStack", "(J)[" LUAVALUE_CLASS, (void *)jni_dumpStack},
    {"_removeStack", "(JI)V", (void *)jni_removeStack},
    {"_pop", "(JI)V", (void *)jni_pop},
    {"_getTop", "(J)I", (void *)jni_getTop},
    {"_removeNativeValue", "(J" STRING_CLASS "I)V", (void *)jni_removeNativeValue},

    {"_loadData", "(J" STRING_CLASS "[B)I", (void *)jni_loadData},
    {"_loadFile", "(J" STRING_CLASS "" STRING_CLASS ")I", (void *)jni_loadFile},
    {"_doLoadedData", "(J)I", (void *)jni_doLoadedData},
    {"_startDebug", "(J[B" STRING_CLASS "I)I", (void *)jni_startDebug},
    {"_setMainEntryFromPreload", "(J" STRING_CLASS ")Z", (void *)jni_setMainEntryFromPreload},
    {"_preloadData", "(J" STRING_CLASS "[B)V", (void *)jni_preloadData},
    {"_preloadFile", "(J" STRING_CLASS "" STRING_CLASS ")V", (void *)jni_preloadFile},

    {"_createTable", "(J)" STRING_CLASS, (void *)jni_createTable},
    {"_getTableSize", "(J" STRING_CLASS ")I", (void *)jni_getTableSize},

    {"_setTableNumber", "(J" STRING_CLASS "ID)V", (void *)jni_setTableNumber},
    {"_setTableBoolean", "(J" STRING_CLASS "IZ)V", (void *)jni_setTableBoolean},
    {"_setTableString", "(J" STRING_CLASS "I" STRING_CLASS ")V", (void *)jni_setTableString},
    {"_setTableNil", "(J" STRING_CLASS "I)V", (void *)jni_setTableNil},
    {"_setTableChild", "(J" STRING_CLASS "I" OBJECT_CLASS "I)V", (void *)jni_setTableChild},

    {"_setTableNumber", "(J" STRING_CLASS "" STRING_CLASS "D)V", (void *)jni_setTableSNumber},
    {"_setTableBoolean", "(J" STRING_CLASS "" STRING_CLASS "Z)V", (void *)jni_setTableSBoolean},
    {"_setTableString", "(J" STRING_CLASS "" STRING_CLASS "" STRING_CLASS ")V", (void *)jni_setTableSString},
    {"_setTableNil", "(J" STRING_CLASS "" STRING_CLASS ")V", (void *)jni_setTableSNil},
    {"_setTableChild", "(J" STRING_CLASS "" STRING_CLASS "" OBJECT_CLASS "I)V", (void *)jni_setTableSChild},

    {"_getTableValue", "(J" STRING_CLASS "I)" OBJECT_CLASS, (void *)jni_getTableValue},
    {"_getTableValue", "(J" STRING_CLASS "" STRING_CLASS ")" OBJECT_CLASS, (void *)jni_getTableSValue},

    {"_createUserdataAndSet", "(J" STRING_CLASS "" STRING_CLASS "[" LUAVALUE_CLASS ")" OBJECT_CLASS, (void *)jni_createUserdataAndSet},

    {"_getTableEntry", "(J" STRING_CLASS ")" OBJECT_CLASS, (void *)jni_getTableEntry},
    {"_startTraverseTable", "(J" STRING_CLASS ")Z", (void *)jni_startTraverseTable},
    {"_nextEntry", "(JZ)[" LUAVALUE_CLASS, (void *)jni_nextEntry},
    {"_endTraverseTable", "(J)V", (void *)jni_endTraverseTable},

    {"_invoke", "(J" STRING_CLASS "[" LUAVALUE_CLASS "I)[" LUAVALUE_CLASS, (void *)jni_invoke},
    {"_registerStaticClassSimple", "(J" STRING_CLASS "" STRING_CLASS "" STRING_CLASS "[" STRING_CLASS ")V", (void *)jni_registerStaticClassSimple},
    {"_registerStaticClass", "(J" STRING_CLASS "" STRING_CLASS "[" STRING_CLASS "[" STRING_CLASS "[I)V", (void *)jni_registerStaticClass},
    {"_registerUserdata", "(J" STRING_CLASS "" STRING_CLASS "" STRING_CLASS "[" STRING_CLASS ")V", (void *)jni_registerUserdata},
    {"_registerAllUserdata", "(J[" STRING_CLASS "[" STRING_CLASS "[" STRING_CLASS "[I[Z[" STRING_CLASS ")V", (void *)jni_registerAllUserdata},
    {"_registerUserdataLazy", "(J" STRING_CLASS "" STRING_CLASS "" STRING_CLASS "[" STRING_CLASS ")V", (void *)jni_registerUserdataLazy},

    {"_registerNumberEnum", "(J" STRING_CLASS "[" STRING_CLASS "[D)V", (void *)jni_registerNumberEnum},
    {"_registerStringEnum", "(J" STRING_CLASS "[" STRING_CLASS "[" STRING_CLASS ")V", (void *)jni_registerStringEnum},

};
#endif //LUA_J_API_H