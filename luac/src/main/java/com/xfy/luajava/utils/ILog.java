package com.xfy.luajava.utils;

/**
 * Created by Xiong.Fangyu on 2019/3/5
 *
 * @see NativeLog
 */
public interface ILog {
    /**
     * 打印日志
     * @param L 虚拟机地址，可通过 {@link com.xfy.luajava.Globals#getGlobalsByLState(long)}获取
     */
    void l(long L, String tag, String log);

    /**
     * 打印错误日志
     * @param L 虚拟机地址，可通过 {@link com.xfy.luajava.Globals#getGlobalsByLState(long)}获取
     */
    void e(long L, String tag, String log);
}
