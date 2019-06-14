package com.xfy.luacdemo.view;

import android.graphics.Color;

import com.xfy.luajava.Globals;
import com.xfy.luajava.JavaUserdata;
import com.xfy.luajava.LuaUserdata;
import com.xfy.luajava.LuaValue;

/**
 * Created by Xiong.Fangyu on 2019/3/11
 */
public class UDColor extends LuaUserdata {
    /**
     * 必须有传入long和LuaValue[]的构造方法，且不可混淆
     *
     * @param L 虚拟机地址
     * @param v lua脚本传入的构造参数
     */
    protected UDColor(long L, LuaValue[] v) {
        super(L, v);
        if (v != null && v.length == 4) {
            color = Color.argb((int) (v[3].toDouble() * 255), v[0].toInt(), v[1].toInt(), v[2].toInt());
        }
    }

    public UDColor(Globals g) {
        super(g, null);
    }

    public int color;

    public int getColor() {
        return color;
    }

    @Override
    public String toString() {
        return "color-"+color;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        UDColor udColor = (UDColor) o;
        return color == udColor.color;
    }

    @Override
    public int hashCode() {
        return color;
    }
}
