package com.xfy.luacdemo.view;

import android.view.Gravity;

import com.xfy.luajava.wrapper.Constant;
import com.xfy.luajava.wrapper.ConstantClass;

/**
 * Created by Xiong.Fangyu on 2019/4/24
 */
@ConstantClass(alias = "Gravity")
public interface GravityE {
    @Constant
    int RIGHT = Gravity.RIGHT;
    @Constant
    int BOTTOM = Gravity.BOTTOM;
}
