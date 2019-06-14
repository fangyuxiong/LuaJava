package com.xfy.luajava;

import com.xfy.luajava.utils.LuaApiUsed;

/**
 * Created by Xiong.Fangyu on 2019/2/25
 * <p>
 * 含有Lua栈位置信息的类型
 *
 * 使用完成及时调用{@link #destroy()}方法
 *
 * @see LuaTable
 * @see LuaFunction
 * @see LuaUserdata
 * @see LuaThread
 */
@LuaApiUsed
abstract class NLuaValue extends LuaValue {
    /**
     * 若相关数据保存到native GNV表中，则key不为空
     */
    @LuaApiUsed
    String nativeGlobalKey;
    /**
     * 虚拟机
     */
    protected Globals globals;

    /**
     * 创建Table or UserData时使用
     * @see LuaTable#create
     * @see LuaUserdata
     */
    NLuaValue(Globals globals, String nativeGlobalKey) {
        this.globals = globals;
        this.nativeGlobalKey = nativeGlobalKey;
    }

    /**
     * For Globals
     * @see Globals
     */
    NLuaValue(String nativeGlobalKey) {
        this.nativeGlobalKey = nativeGlobalKey;
    }

    /**
     * Called by native method.
     */
    @LuaApiUsed
    NLuaValue(long L_state, String nativeGlobalKey) {
        this(nativeGlobalKey);
        this.globals = Globals.getGlobalsByLState(L_state);
    }

    /**
     * 返回当前数据在GNV表中的key
     * @see #nativeGlobalKey
     */
    public String nativeGlobalKey() {
        return nativeGlobalKey;
    }

    /**
     * 销毁native状态
     * table、function、userdata将从GVN表中移除
     * globals将销毁虚拟机
     * @see Globals#destroy()
     * @see Globals#removeStack(LuaValue)
     */
    public void destroy() {
        if (destroyed || globals.destroyed)
            return;
        destroyed = globals.removeStack(this);
    }

    /**
     * 获取虚拟机信息
     */
    public Globals getGlobals() {
        return globals;
    }

    @Override
    @LuaApiUsed
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        NLuaValue luaValue = (NLuaValue) o;
        if (nativeGlobalKey == null)
            return luaValue.nativeGlobalKey == null;
        if (!nativeGlobalKey.equals(Globals.GLOBALS_INDEX)) {
            return nativeGlobalKey.equals(luaValue.nativeGlobalKey);
        } else {//LuaUserData java层创建的会使用global的nativeGlobalKey: "GLOBALS_INDEX"。不能进行比较
            return super.equals(o);
        }
    }

    @Override
    public int hashCode() {
        if (nativeGlobalKey != null)
            return nativeGlobalKey.hashCode();
        return super.hashCode();
    }

    @Override
    public String toString() {
        return LUA_TYPE_NAME[type()] + "#(" + nativeGlobalKey + ")";
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            destroy();
        } finally {
            super.finalize();
        }
    }
}
