package com.xfy.luajava.wrapper;

import com.xfy.luajava.Globals;
import com.xfy.luajava.LuaUserdata;
import com.xfy.luajava.LuaValue;

/**
 * Created by Xiong.Fangyu on 2019/3/19
 *
 * lua数据类型转换接口
 */
public interface ILuaValueGetter<L extends LuaValue, O> {
    /**
     * 不可返回null
     * 使用 {@link LuaValue#Nil()} 代替null
     * @param g     虚拟机信息
     * @param obj   原始数据
     * @return nonnull
     */
    L newInstance(Globals g, O obj);
}
