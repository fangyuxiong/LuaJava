package com.xfy.luajava.utils;

import com.xfy.luajava.LuaConfigs;

import java.util.HashMap;

/**
 * Created by Xiong.Fangyu on 2019/2/27
 * <p>
 * called by native
 * <p>
 * lua的print函数或错误日志，都将通过{@link #log}方法打印
 */
@LuaApiUsed
public final class NativeLog {
    private static final String TAG = "NativeLog";

    private static final HashMap<Long, StringBuilder> logBuilder = new HashMap<>();
    private static final HashMap<Long, ILog> logs = new HashMap<>();

    private static StringBuilder get(long L) {
        StringBuilder sb = logBuilder.get(L);
        if (sb == null) {
            sb = new StringBuilder();
            logBuilder.put(L, sb);
        }
        return sb;
    }

    public static void release(long L) {
        logBuilder.remove(L);
        logs.remove(L);
    }

    public static void register(long L, ILog log) {
        logs.put(L, log);
    }

    /**
     * called by native
     * see jlog.c
     */
    @LuaApiUsed
    private static void log(long L, int type, String s) {
        switch (type) {
            case 1:     //log
                get(L).append(s);
                break;
            case -1:    //flush
                l(L);
                break;
            case 2:     //Error log
                le(L, s);
                break;
        }
    }

    private static void l(long L) {
        String log = get(L).toString();
        get(L).setLength(0);
        if (LuaConfigs.openLogLevel >= LuaConfigs.LOG_ALL) {
            ILog logImpl = logs.get(L);
            if (logImpl != null) {
                logImpl.l(L, TAG, log);
            } else {
                System.out.println(TAG + " " + log);
            }
        }
    }

    private static void le(long L, String s) {
        if (LuaConfigs.openLogLevel >= LuaConfigs.LOG_ERR) {
            ILog logImpl = logs.get(L);
            if (logImpl != null) {
                logImpl.e(L, TAG, s);
            } else {
                System.out.println(TAG + " " + s);
            }
        }
    }
}
