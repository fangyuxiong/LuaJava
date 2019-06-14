package com.xfy.luacdemo.wrapper;

import com.xfy.annotation.LuaBridge;
import com.xfy.annotation.LuaClass;
import com.xfy.luajava.Globals;
import com.xfy.luajava.LuaValue;
import com.xfy.luajava.wrapper.callback.IVoidCallback;

/**
 * Created by Xiong.Fangyu on 2019/3/19
 */
/// 标记此类为非静态(userdata)类型，且内存由lua控制
/// 将会生成 继承 LuaUserdata(内存由lua控制) 的 Size_udwrapper 类
@LuaClass(isStatic = false, gcByLua = true)
public class Size {
    /// 必须有此构造方法
    public Size(Globals g, LuaValue[] init) {}

    private IVoidCallback callback;

    public Size(double w, double h) {
        this.width = w;
        this.height = h;
    }
    /// 标记此属性可被lua通过 w方法调用
    /// 若方法中不传参数，则表示getter方法，若传参数，则表示setter方法
    @LuaBridge(alias = "w")
    double width;
    @LuaBridge(alias = "h")
    double height;

    /// 标记此方法可被lua通过 area方法调用
    @LuaBridge
    public double area() {
        return width * height;
    }

    @LuaBridge
    public void triggerCallback() {
        if (callback != null) {
            callback.callback();
        }
    }

    @LuaBridge
    public void setCallback(IVoidCallback callback) {
        this.callback = callback;
    }

    public boolean equals(Object other) {
        if (this == other) return true;
        if (other == null) return false;
        if (getClass() != other.getClass()) return false;
        Size os = (Size)other;
        return os.width == width && os.height == height;
    }
}
