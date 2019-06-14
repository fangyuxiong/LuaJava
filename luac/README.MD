## 使用方法

#### 加载脚本、执行脚本：
```
Globals globals = Globals.createLState(debugable); //创建Lua虚拟机
globals.registerStaticBridgeSimple("LuaBridgeName", JavaClass.class, ...); //注册Java静态方法
globals.registerUserdataSimple("LuaBridgeName2", JavaClass2.class, ...); //注册Java对象，并生成userdata

boolean result = globals.loadString("name", str);//加载lua源码
boolean result = globals.loadData("name", data); //加载lua源码或二进制码
/// 二进制码必须在相同的机器环境下编译
// globals.compileAndSave(file, luaData);//将Lua源码编译成二进制码，并保存到file中

if (result)
    result = globals.callLoadedData();//执行刚加载成功的lua脚本
```

#### 静态Bridge编写
```
@LuaApiUsed //增加这个注释，并保证此类不会被混淆
class StaticBridge {
    /// 方法参数必须为long, LuaValue[]类型
    /// 返回值类型必须为LuaValue[]
    @LuaApiUsed
    static LuaValue[] bridgeA(long L, LuaValue[] p) {
        // code
        return null;//可返回空
    }
}
```

#### Userdata编写

继承`LuaUserdata`，根据暴露接口编写；若需要Java层管理内存，则继承`JavaUserdata`，且在使用完成后，调用`destroy`方法
```
@LuaApiUsed
class MyUserdata extends LuaUserdata {
    @LuaApiUsed
    MyUserdata(long L, LuaValue[] v) {
        super(L, v);
    }

    /// 方法参数和返回值类型必须为LuaValue[]
    @LuaApiUsed
    LuaValue[] bridgeA(LuaValue[] p) {
        // code
        return null; // 可返回空
    }
}

globals.registerUserdataSimple("MyUserdataName", MyUserdata.class, "bridgeA"); //注册userdata
```

#### 混淆注意
```
-keep class com.xfy.luajava.utils.LuaApiUsed
-keep @com.xfy.luajava.utils.LuaApiUsed class * {
    native <methods>;
    @com.xfy.luajava.utils.LuaApiUsed <methods>;
    @com.xfy.luajava.utils.LuaApiUsed <fields>;
}
```