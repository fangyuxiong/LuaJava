package com.xfy.luajava;


/**
 * Created by Xiong.Fangyu on 2019/3/5
 * <p>
 * 配置信息
 */
public class LuaConfigs {
    public static final byte LOG_CLOSE = 0; //关闭日志
    public static final byte LOG_ERR = 1; //关闭普通日志，开启错误日志
    public static final byte LOG_ALL = 2; //开启所有日志
    /**
     * 日志开启状态
     */
    public static byte openLogLevel = LOG_ALL;
}
