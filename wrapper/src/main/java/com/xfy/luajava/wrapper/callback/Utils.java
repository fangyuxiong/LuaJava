package com.xfy.luajava.wrapper.callback;

import com.xfy.luajava.Globals;
import com.xfy.luajava.LuaFunction;
import com.xfy.luajava.LuaValue;
import com.xfy.luajava.wrapper.Translator;

/**
 * Created by Xiong.Fangyu on 2019/3/21
 */
class Utils {

    static void check(LuaFunction luaFunction) {
        if (luaFunction == null || luaFunction.isDestroyed()) {
            throw new IllegalStateException("lua function is destroyed.");
        }
    }

    static LuaValue[] invoke(LuaFunction luaFunction, Object... params) {
        final int len = params == null ? 0 : params.length;
        if (len == 0) {
            return luaFunction.invoke(null);
        }
        if (params.getClass() == LuaValue[].class) {
            return luaFunction.invoke((LuaValue[]) params);
        }
        LuaValue[] p = new LuaValue[len];
        Globals globals = luaFunction.getGlobals();
        for (int i = 0; i < len; i++) {
            Object pn = params[i];
            if (pn == null) {
                p[i] = LuaValue.Nil();
                continue;
            }
            if (pn instanceof LuaValue) {
                p[i] = (LuaValue) pn;
                continue;
            }
            if (Translator.isPrimitiveLuaData(pn)) {
                p[i] = Translator.translatePrimitiveToLua(pn);
                continue;
            }
            p[i] = Translator.translateJavaToLua(globals, params[i]);
        }
        return luaFunction.invoke(p);
    }
}
