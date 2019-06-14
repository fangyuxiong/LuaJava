package com.xfy.luajava;

import com.xfy.luajava.utils.LuaApiUsed;

/**
 * Created by Xiong.Fangyu on 2019/2/22
 * <p>
 * Lua 线程，暂无接口
 */
@LuaApiUsed
public class LuaThread extends NLuaValue {

    @LuaApiUsed
    LuaThread(long L_state, String stackIndex) {
        super(L_state, stackIndex);
    }

    @Override
    public int type() {
        return LUA_TTHREAD;
    }

    public LuaThread toLuaThread() {
        return this;
    }
}
