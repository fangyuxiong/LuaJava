package com.xfy.luacdemo.wrapper;

import com.xfy.annotation.LuaBridge;
import com.xfy.annotation.LuaClass;

/**
 * Created by Xiong.Fangyu on 2019/3/19
 */
/// 标记此类为静态(table)类型
/// 将会生成 AreaUtils_sbwrapper 类
@LuaClass(isStatic = true)
public class AreaUtils {
    /// 标记此方法可被lua通过 newSize方法调用
    /// 并返回给lua一个userdata(Size)
    @LuaBridge
    static Size newSize(double w, double h) {
        return new Size(w, h);
    }

    /// 标记此方法可被lua通过 calSizeArea方法调用
    @LuaBridge(alias = "calSizeArea")
    static double csa(Size s) {
        return s.area();
    }
}