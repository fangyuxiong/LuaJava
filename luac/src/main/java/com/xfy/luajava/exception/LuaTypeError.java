package com.xfy.luajava.exception;

import com.xfy.luajava.utils.LuaApiUsed;

/**
 * Created by Xiong.Fangyu on 2019/2/21
 * <p>
 * Lua类型转换错误
 *
 * @see com.xfy.luajava.LuaValue
 */
@LuaApiUsed
public class LuaTypeError extends RuntimeException {

    public LuaTypeError() {
    }

    public LuaTypeError(Throwable cause) {
        super(cause);
    }

    public LuaTypeError(String msg) {
        super(msg);
    }

    public LuaTypeError(String msg, Throwable cause) {
        super(msg, cause);
    }
}
