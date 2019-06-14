package com.xfy.luacdemo.view;

import com.xfy.luajava.LuaNumber;
import com.xfy.luajava.LuaValue;

/**
 * Created by Xiong.Fangyu on 2019/3/11
 */
public class MBit {
    static LuaValue[] bor(long J, LuaValue[] p) {
        return LuaValue.varargsOf(LuaNumber.valueOf(p[0].toInt() | p[1].toInt()));
    }
}
