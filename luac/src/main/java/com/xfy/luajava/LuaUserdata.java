package com.xfy.luajava;

import com.xfy.luajava.utils.LuaApiUsed;

/**
 * Created by Xiong.Fangyu on 2019/2/22
 * <p>
 * Lua Userdata
 *
 * 若想让Java层管理内存，继承{@link JavaUserdata}
 *
 */
@LuaApiUsed
public class LuaUserdata extends NLuaValue {
    /**
     * 由Native读取
     */
    @LuaApiUsed
    private String luaclassName;
    /**
     * 在java中保存的对象
     */
    protected Object javaUserdata;

    /**
     * 由java层创建
     * @param g         虚拟机信息
     * @param jud       java中保存的对象，可为空
     * @see #javaUserdata
     */
    public LuaUserdata(Globals g, Object jud) {
        super(g, g.globalsIndex());
        luaclassName = initLuaClassName(g);
        javaUserdata = jud;
    }

    /**
     * 必须有传入long和LuaValue[]的构造方法，且不可混淆
     * 由native创建
     *
     * 子类可在此构造函数中初始化{@link #javaUserdata}
     *
     * 必须有此构造方法！！！！！！！！
     *
     * @param L 虚拟机地址
     * @param v lua脚本传入的构造参数
     */
    @LuaApiUsed
    protected LuaUserdata(long L, LuaValue[] v) {
        super(L, null);
    }

    /**
     * 初始化{@link #luaclassName}
     * 默认从虚拟机中注册表中获取
     * 若没继承，子类需要自己实现
     * @param g 虚拟机
     */
    protected String initLuaClassName(Globals g) {
        return g.getLuaClassName(getClass());
    }

    /**
     * 此对象被Lua GC时会调用
     */
    @LuaApiUsed
    protected void __onLuaGc() { }

    /**
     * Lua中调用 == 时会调用
     */
    @LuaApiUsed
    protected boolean __onLuaEq(Object other) {
        return equals(other);
    }

    /**
     * 由Native创建
     */
    @LuaApiUsed
    private LuaUserdata(long L_state, String stackIndex) {
        super(L_state, stackIndex);
    }

    @Override
    public int type() {
        return LUA_TUSERDATA;
    }

    @Override
    public LuaUserdata toUserdata() {
        return this;
    }

    /**
     * 返回java中保存的对象
     * @return Nullable
     */
    public Object getJavaUserdata() {
        return javaUserdata;
    }

    @Override
    public int hashCode() {
        if (javaUserdata != null)
            return javaUserdata.hashCode();
        return super.hashCode();
    }
}
