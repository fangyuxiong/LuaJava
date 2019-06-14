package com.xfy.luajava.wrapper;

/**
 * Created by Xiong.Fangyu on 2019/3/18
 */
public class RegisterError extends RuntimeException {

    public RegisterError(Throwable c) {
        super(c);
    }

    public RegisterError(String msg) {
        super(msg);
    }

    public RegisterError(String msg, Throwable e) {
        super(msg, e);
    }
}
