package com.xfy.luajava.exception;

import com.xfy.luajava.utils.LuaApiUsed;

/**
 * Created by Xiong.Fangyu on 2019/2/26
 * <p>
 * 调用函数报错，由Native抛出
 */
@LuaApiUsed
public class InvokeError extends RuntimeException {
    @LuaApiUsed
    public InvokeError(String msg) {
        super(msg);
    }

    @LuaApiUsed
    public InvokeError(String msg, Throwable t) {
        super(msg, t);
    }
}
