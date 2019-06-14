package com.xfy.luajava.wrapper.callback;

import com.xfy.luajava.LuaFunction;
import com.xfy.luajava.LuaValue;
import com.xfy.luajava.wrapper.IJavaObjectGetter;

/**
 * Created by Xiong.Fangyu on 2019/3/21
 *
 * 封装{@link com.xfy.luajava.LuaFunction}的接口
 *
 * 原始回调Lua方法，返回值为boolean类型
 */
public class DefaultBoolCallback implements IBoolCallback {

    protected LuaFunction luaFunction;

    public DefaultBoolCallback(LuaFunction f) {
        luaFunction = f;
    }

    public static final IJavaObjectGetter<LuaFunction, IBoolCallback> G = new IJavaObjectGetter<LuaFunction, IBoolCallback>() {
        @Override
        public DefaultBoolCallback getJavaObject(LuaFunction lv) {
            return new DefaultBoolCallback(lv) ;
        }
    };

    @Override
    public boolean callback(Object... params) {
        Utils.check(luaFunction);
        LuaValue[] r = Utils.invoke(luaFunction, params);
        return r[0].toBoolean();
    }

    @Override
    public boolean callbackAndDestroy(Object... params) {
        Utils.check(luaFunction);
        LuaValue[] r = Utils.invoke(luaFunction, params);
        luaFunction.destroy();
        luaFunction = null;
        return r[0].toBoolean();
    }

    @Override
    public void destroy() {
        if (luaFunction != null)
            luaFunction.destroy();
        luaFunction = null;
    }
}
