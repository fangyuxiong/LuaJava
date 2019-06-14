package com.xfy.luajava.wrapper.callback;

/**
 * Created by Xiong.Fangyu on 2019/3/21
 *
 * 封装{@link com.xfy.luajava.LuaFunction}的接口
 *
 * 回调Lua方法，返回值为boolean类型
 */
public interface IBoolCallback extends Destroyable{
    /**
     * 回调lua方法
     * @param params 参数
     */
    boolean callback(Object... params);

    /**
     * 回调lua方法，调用之后，将不能再次使用此回调
     * @param params 参数
     */
    boolean callbackAndDestroy(Object... params);
}
