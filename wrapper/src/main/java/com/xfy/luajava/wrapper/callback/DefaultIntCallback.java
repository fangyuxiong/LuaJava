package com.xfy.luajava.wrapper.callback;

import com.xfy.luajava.LuaFunction;
import com.xfy.luajava.LuaValue;
import com.xfy.luajava.wrapper.IJavaObjectGetter;

/**
 * Created by Xiong.Fangyu on 2019/3/21
 *
 * 封装{@link com.xfy.luajava.LuaFunction}的接口
 *
 * 原始回调Lua方法，返回值为int数字类型
 */
public class DefaultIntCallback implements IIntCallback {

    private LuaFunction luaFunction;

    public DefaultIntCallback(LuaFunction f) {
        luaFunction = f;
    }

    public static final IJavaObjectGetter<LuaFunction, IIntCallback> G = new IJavaObjectGetter<LuaFunction, IIntCallback>() {
        @Override
        public IIntCallback getJavaObject(LuaFunction lv) {
            return new DefaultIntCallback(lv) ;
        }
    };

    @Override
    public int callback(Object... params) {
        Utils.check(luaFunction);
        LuaValue[] r = Utils.invoke(luaFunction, params);
        return r[0].toInt();
    }

    @Override
    public int callbackAndDestroy(Object... params) {
        Utils.check(luaFunction);
        LuaValue[] r = Utils.invoke(luaFunction, params);
        luaFunction.destroy();
        luaFunction = null;
        return r[0].toInt();
    }

    @Override
    public void destroy() {
        if (luaFunction != null)
            luaFunction.destroy();
        luaFunction = null;
    }
}
