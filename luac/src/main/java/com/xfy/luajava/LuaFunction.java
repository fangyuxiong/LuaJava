package com.xfy.luajava;

import com.xfy.luajava.exception.InvokeError;
import com.xfy.luajava.utils.LuaApiUsed;

/**
 * Created by Xiong.Fangyu on 2019/2/22
 * <p>
 * Lua函数封装类
 * 可使用{@link #invoke(LuaValue[])} {@link #invoke(LuaValue[], int)}调用
 * <p>
 * 通过注册静态Bridge代替
 *
 * @see Globals#registerStaticBridgeSimple
 * @see Globals#registerStaticBridge
 */
@LuaApiUsed
public class LuaFunction extends NLuaValue {
    // Lua返回可变参数个数
    private static final int LUA_MULTRET = -1;

    /**
     * Called by native method.
     * see luajapi.c
     */
    @LuaApiUsed
    private LuaFunction(long L_state, String stackIndex) {
        super(L_state, stackIndex);
    }

    @Override
    public int type() {
        return LUA_TFUNCTION;
    }

    @Override
    public LuaFunction toLuaFunction() {
        return this;
    }

    /**
     * 调用lua方法，且已知方法返回个数
     *
     * @param params      方法传入的参数，可为空
     * @param returnCount 方法返回参数个数
     * @return 返回参数，若returnCount为0，返回空
     */
    public LuaValue[] invoke(LuaValue[] params, int returnCount) throws InvokeError {
        try {
            if (!checkStatus())
                return empty();
            globals.calledFunction ++;
            LuaValue[] ret = LuaCApi._invoke(globals.L_State, nativeGlobalKey, params, returnCount);
            globals.calledFunction --;
            return ret;
        } catch (InvokeError e) {
            globals.calledFunction --;
            throw e;
        }
    }

    /**
     * 调用lua方法，方法返回参数个数未知
     *
     * @param params 方法传入的参数，可为空
     * @return 返回参数，可为空
     */
    public LuaValue[] invoke(LuaValue[] params) throws InvokeError {
        try {
            if (!checkStatus())
                return empty();
            globals.calledFunction ++;
            LuaValue[] ret =  LuaCApi._invoke(globals.L_State, nativeGlobalKey, params, LUA_MULTRET);
            globals.calledFunction --;
            return ret;
        } catch (InvokeError e) {
            globals.calledFunction --;
            throw e;
        }
    }

    /**
     * 检查function的状态，若在debug状态中，或在加载主脚本的状态，则抛出异常
     * @return true: 可正常执行
     */
    private boolean checkStatus() {
        if (destroyed) {
            throw new InvokeError("function is destroyed");
        }
        if (globals.isDestroyed()) {
            throw new InvokeError("globals is destroyed");
        }
        return true;
    }

}
