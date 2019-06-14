## 使用方法

gradle依赖：
```
implementation project(':annotation')       //注解依赖
implementation project(':wrapper')          //帮助依赖
annotationProcessor project(':lprocess')    //Android编译期注入代码库
```

#### 原理

通过Android编译期注入技术(annotationProcessor)，将带有注解的类和方法，使用lua可识别的数据包裹，达到简化开发的效果。

**[不使用此方式的Bridge开发](../luac/README.MD)**

**使用后的Userdata开发：**

java code
```
/// Size.java
/// 标记此类为非静态(userdata)类型，且内存由lua控制
/// 将会生成 继承 LuaUserdata(内存由lua控制) 的 Size_udwrapper 类
@LuaClass(isStatic = false, gcByLua = true)
public class Size {
    /// 必须有此构造方法
    public Size(Globals g, LuaValue[] init) {}

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

    public boolean equals(Object other) {
        if (this == other) return true;
        if (other == null) return false;
        if (getClass() != other.getClass()) return false;
        Size os = (Size)other;
        return os.width == width && os.height == height;
    }
}

/// AreaUtils.java
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

/// 初始化，如Application中
/// 提前注册userdata相关信息
Register.registerUserdata(Size.class, lazyLoad, "Size");
/// 提前注册静态接口相关信息
Register.registerStaticBridge("AreaUtils", AreaUtils.class);
/// 虚拟机创建后，将缓存的注册信息安装到相应lua环境中
Register.install(globals);

/// 自动类型转换机制，若未调用下面方法，则AreaUtils两个方法均不可用
/// 自动将Lua数据类型userdata(Size)转换成java数据类型Size
UserdataTranslator.registerL2JAuto(Size.class);
/// 自动将java数据类型Size转换成lua数据类型userdata(Size)
UserdataTranslator.registerJ2LAuto(Size.class);
```

lua code

```
local size = Size()
--- set (Java)Size.width = 1
size:w(1)
--- get (Java)Size.width, set (Java)Size.width to (Java)Size.height
size:h(size:w())
--- call (Java)Size.area() return number value
local area = size:area()

--- call (Java)AreaUtils.newSize return userdata value
local otherSize = AreaUtils:newSize(1,1)
--- will call (Java)Size.equals
print('is eqauls:', size == otherSize)
--- call (Java)AreaUtils.csa
print('area by AreaUtils', AreaUtils:calSizeArea(otherSize))
```