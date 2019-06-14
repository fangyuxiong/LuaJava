package com.xfy.luacdemo;

import android.util.Log;

import com.xfy.luajava.Globals;
import com.xfy.luajava.LuaNumber;
import com.xfy.luajava.LuaString;
import com.xfy.luajava.LuaValue;
import com.xfy.luajava.utils.LuaApiUsed;

/**
 * Created by Xiong.Fangyu on 2019/2/27
 */
@LuaApiUsed
public class LuaStaticClassTest {

    @LuaApiUsed
    public static LuaValue[] a(LuaValue[] a) {
        return new LuaValue[] {
                LuaString.valueOf("p: " + a[0].toJavaString()),
                LuaNumber.valueOf(2)
        };
    }
    @LuaApiUsed
    public static LuaValue[] b(LuaValue[] a) {
        return null;
    }
    @LuaApiUsed
    public static LuaValue[] c(LuaValue[] a) {
        return null;
    }
    @LuaApiUsed
    public static LuaValue[] d(LuaValue[] a) {
        return null;
    }
    @LuaApiUsed
    public static LuaValue[] e(LuaValue[] a) {
        return null;
    }
    @LuaApiUsed
    public static LuaValue[] f(LuaValue[] a) {
        return null;
    }
}
