package com.xfy.luajava;

import com.xfy.luajava.utils.LuaApiUsed;

/**
 * Created by Xiong.Fangyu on 2019/2/21
 *
 * Lua boolean 数据
 * 通过 {@link LuaValue#True()} {@link LuaValue#False()}获取
 *
 * 不要随意修改！see luajapi.c
 */
@LuaApiUsed
final class LuaBoolean extends LuaValue {

    private static volatile LuaBoolean TRUE;
    private static volatile LuaBoolean FALSE;

    @LuaApiUsed
    static LuaBoolean TRUE() {
        if (TRUE == null) {
            synchronized (LuaBoolean.class) {
                if (TRUE == null) {
                    TRUE = new LuaBoolean(true);
                }
            }
        }
        return TRUE;
    }

    @LuaApiUsed
    static LuaBoolean FALSE() {
        if (FALSE == null) {
            synchronized (LuaBoolean.class) {
                if (FALSE == null) {
                    FALSE = new LuaBoolean(false);
                }
            }
        }
        return FALSE;
    }

    @LuaApiUsed
    private final boolean value;

    private LuaBoolean(boolean v) {
        value = v;
    }

    @Override
    public int type() {
        return LUA_TBOOLEAN;
    }

    @Override
    public boolean toBoolean() {
        return value;
    }

    @Override
    public String toJavaString() {
        return String.valueOf(value);
    }

    @Override
    public String toString() {
        return LUA_TYPE_NAME[type()] + "@" + value;
    }
}
