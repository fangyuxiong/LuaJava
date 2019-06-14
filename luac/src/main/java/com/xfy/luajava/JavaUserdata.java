package com.xfy.luajava;

import com.xfy.luajava.utils.LuaApiUsed;

/**
 * Created by Xiong.Fangyu on 2019/3/5
 *
 * 若直接继承此类实现userdata，则实例传给Java层时，将把Native层相应userdata存到GNV表中，不可释放
 * 直到调用{@link #destroy()}方法
 *
 * 若想让lua虚拟机管理userdata内存，可不继承此类或继承{@link JavaUserdata}
 */
@LuaApiUsed
public class JavaUserdata extends LuaUserdata {
    /**
     * 必须有传入long和LuaValue[]的构造方法，且不可混淆
     * 由native创建
     *
     * 必须有此构造方法！！！！！！！！
     *
     * @param L 虚拟机地址
     * @param v lua脚本传入的构造参数
     */
    @LuaApiUsed
    protected JavaUserdata(long L, LuaValue[] v) {
        super(L, v);
    }

    /**
     * 由Java层创建
     *
     * @param g 虚拟机
     * @param jud       java中保存的对象，可为空
     * @see #javaUserdata
     */
    public JavaUserdata(Globals g, Object jud) {
        super(g, jud);
    }
}
