package com.xfy.luacdemo;

import android.util.Log;

import com.xfy.luajava.Globals;
import com.xfy.luajava.JavaUserdata;
import com.xfy.luajava.LuaValue;

/**
 * Created by Xiong.Fangyu on 2019/3/11
 */
public class LazyUserdata extends JavaUserdata {
    private static final String TAG = "LazyUserdata";
    /**
     * 必须有传入long和LuaValue[]的构造方法，且不可混淆
     *
     * @param L 虚拟机地址
     * @param v lua脚本传入的构造参数
     */
    public LazyUserdata(long L, LuaValue[] v) {
        super(L, v);
        Log.d(TAG, "LazyUserdata: init");
    }

    public LazyUserdata(Globals g) {
        super(g.getL_State(), null);
    }

    LuaValue[] a(LuaValue[] p) {
        return p;
    }
}
