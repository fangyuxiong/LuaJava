package com.xfy.luajava.exception;

import com.xfy.luajava.utils.LuaApiUsed;

/**
 * Created by Xiong.Fangyu on 2019/3/14
 *
 * for native
 */
@LuaApiUsed
public class UndumpError extends RuntimeException {

    @LuaApiUsed
    public UndumpError(String msg) {
        super(msg);
    }
}
