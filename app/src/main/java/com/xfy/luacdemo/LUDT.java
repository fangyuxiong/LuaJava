package com.xfy.luacdemo;

import com.xfy.luajava.JavaUserdata;
import com.xfy.luajava.LuaUserdata;
import com.xfy.luajava.LuaValue;
import com.xfy.luajava.utils.LuaApiUsed;


/**
 * Created by Xiong.Fangyu on 2019/3/1
 */
@LuaApiUsed
public class LUDT extends JavaUserdata {
    private LuaValue[] init;

    /**
     *
     * @param L
     * @param v
     */
    @LuaApiUsed
    LUDT(long L, LuaValue[] v) {
        super(L, v);
        init = v;
    }

    @LuaApiUsed
    LuaValue[] a(final LuaValue[] p) {
        return new LuaValue[] {
                new LuaUserdataExtends(globals, null)
        };
//        return p[0].invoke(null);
    }

    @LuaApiUsed
    @Override
    public String toString() {
        return "Java--LUDT";
    }
}
