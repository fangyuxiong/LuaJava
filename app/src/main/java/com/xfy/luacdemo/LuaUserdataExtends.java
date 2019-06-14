package com.xfy.luacdemo;

import android.util.Log;

import com.xfy.luajava.Globals;
import com.xfy.luajava.JavaUserdata;
import com.xfy.luajava.LuaFunction;
import com.xfy.luajava.LuaNumber;
import com.xfy.luajava.LuaString;
import com.xfy.luajava.LuaValue;
import com.xfy.luajava.utils.LuaApiUsed;

import java.util.Arrays;

/**
 * Created by Xiong.Fangyu on 2019/3/4
 */
@LuaApiUsed
public class LuaUserdataExtends extends JavaUserdata {
    @LuaApiUsed
    LuaUserdataExtends(long L, LuaValue[] v) {
        super(L, v);
    }

    public LuaUserdataExtends(Globals g, Object jud) {
        super(g, jud);
    }

    @LuaApiUsed
    LuaValue[] a(LuaValue[] p) {
        return new LuaValue[] { LuaString.valueOf("extends")};
    }

    @LuaApiUsed
    LuaValue[] b(LuaValue[] p) {
        LuaValue[] r = ((LuaFunction)p[0]).invoke(new LuaValue[] {LuaNumber.valueOf(10)});
        Log.d("eeeeee", "b: " + Arrays.toString(r));
        return r;
    }

    @LuaApiUsed
    LuaValue[] c(LuaValue[] p) {
        Log.d("eeeeee", "c: " + p[0]);
        return null;
    }

    @LuaApiUsed
    protected void __onLuaGc() {
        Log.d("eeeeee", "__onLuaGc: ");
    }

    @LuaApiUsed
    protected boolean __onLuaEq(Object other) {
        if (other instanceof LuaUserdataExtends)
        return true;
        return false;
    }
    @LuaApiUsed
    @Override
    public String toString() {
        return "Java--LuaUserdataExtends";
    }
}
