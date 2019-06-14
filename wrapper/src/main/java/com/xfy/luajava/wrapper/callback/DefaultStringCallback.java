package com.xfy.luajava.wrapper.callback;

import com.xfy.luajava.LuaFunction;
import com.xfy.luajava.LuaValue;
import com.xfy.luajava.wrapper.IJavaObjectGetter;

/**
 * Created by Xiong.Fangyu on 2019/3/21
 *
 * 封装{@link com.xfy.luajava.LuaFunction}的接口
 *
 * 原始回调Lua方法，返回值为String类型
 */
public class DefaultStringCallback implements IStringCallback {

    private LuaFunction luaFunction;

    public DefaultStringCallback(LuaFunction f) {
        luaFunction = f;
    }

    public static final IJavaObjectGetter<LuaFunction, IStringCallback> G = new IJavaObjectGetter<LuaFunction, IStringCallback>() {
        @Override
        public DefaultStringCallback getJavaObject(LuaFunction lv) {
            return new DefaultStringCallback(lv) ;
        }
    };

    @Override
    public String callback(Object... params) {
        Utils.check(luaFunction);
        LuaValue[] r = Utils.invoke(luaFunction, params);
        return r[0].toJavaString();
    }

    @Override
    public String callbackAndDestroy(Object... params) {
        Utils.check(luaFunction);
        LuaValue[] r = Utils.invoke(luaFunction, params);
        luaFunction.destroy();
        luaFunction = null;
        return r[0].toJavaString();
    }

    @Override
    public void destroy() {
        if (luaFunction != null)
            luaFunction.destroy();
        luaFunction = null;
    }
}
