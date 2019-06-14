//
// Created by Xiong.Fangyu 2019/02/27.
//

#include "jlog.h"
#include "mlog.h"

static JNIEnv *jnienv;
// global ref
static jclass NativeLog;
static jmethodID NativeLog_log;
static int isInit = 0;

void initlog(JNIEnv *env)
{
    if (isInit)
        return;
    jnienv = env;
    NativeLog = (*env)->FindClass(env, "com/xfy/luajava/utils/NativeLog");
    NativeLog_log = (*env)->GetStaticMethodID(env, NativeLog, "log", "(JILjava/lang/String;)V");
    NativeLog = (jclass)(*env)->NewGlobalRef(env, NativeLog);
    isInit = 1;
}

void log2java(jlong l, int type, const char *s, void *p)
{
    jstring jstr;
    char str[MAX_STRING_LENGTH];
    if (p)
    {
        if (snprintf(str, MAX_STRING_LENGTH, s, p) != -1)
        {
            jstr = (*jnienv)->NewStringUTF(jnienv, str);
        }
        else
        {
            LOGE("format error! %s", s);
            jstr = (*jnienv)->NewStringUTF(jnienv, "format error!");
        }
    }
    else if (s)
    {
        jstr = (*jnienv)->NewStringUTF(jnienv, s);
    }
    else
    {
        jstr = NULL;
    }
    (*jnienv)->CallStaticVoidMethod(jnienv, NativeLog, NativeLog_log, l, (jint)type, jstr);
}