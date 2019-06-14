package com.xfy.luajava;

import com.xfy.luajava.exception.UndumpError;
import com.xfy.luajava.utils.IGlobalsUserdata;
import com.xfy.luajava.utils.LuaApiUsed;
import com.xfy.luajava.utils.NativeLog;
import com.xfy.luajava.utils.ResourceFinder;
import com.xfy.luajava.utils.SignatureUtils;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Created by Xiong.Fangyu on 2019/2/22
 * <p>
 * Global表
 * <p>
 * @see #createLState               创建全局表
 * @see #setResourceFinder          设置资源查找器
 * @see #loadString                 加载Lua源码
 * @see #loadData                   加载Lua源码或二进制码
 * @see #callLoadedData             执行已加载成功的Lua源码或二进制码
 * @see #getState                   获取加载或执行的状态码
 * @see #compileAndSave             将Lua源码编译成二进制码并保存到文件
 * @see #registerUserdataSimple     注册userdata
 * @see #registerStaticBridgeSimple 注册静态Bridge
 * @see #registerStaticBridge       注册静态Bridge
 * @see #createUserdataAndSet       在全局表中注册一个userdata实例
 * @see #destroy()                  销毁当前虚拟机
 * <p>
 *
 * 资源查找流程：
 * 1、native层，通过接口{@link #setBasePath(String, boolean)}获取根路径，在根路径下查找，若失败，进入java层查找
 * 2、java层，{@link #__onLuaRequire(long, String)} {@link #onRequire(String)}，通过设置的{@link ResourceFinder}查找
 */
@LuaApiUsed
public final class Globals extends LuaTable {
    /**
     * luac 版本号
     */
    public static final int LUAC_VERSION = 83;
    /**
     * Luac 版本
     */
    public static final String LUAC_VERSION_STR = "5.3.3";
    /**
     * Lua所有状态码
     */
    public static final int LUA_OK = 0;
    public static final int LUA_YIELD = 1;
    public static final int LUA_ERRRUN = 2;
    public static final int LUA_ERRSYNTAX = 3;
    public static final int LUA_ERRMEM = 4;
    public static final int LUA_ERRGCMM = 5;
    public static final int LUA_ERRERR = 6;
    public static final int LUA_ERRINJAVA = -1;
    public static final int LUA_CALLING = 100;
    /**
     * 标记底层库是否load完成
     */
    private static boolean init;
    /**
     * 标记底层库的size_t是否是32位
     */
    private static boolean is32bit;
    /**
     * 表示虚拟机状态刚初始化，并未加载脚本或执行脚本
     */
    private static final int JUST_INIT = Integer.MIN_VALUE;

    /**
     * Naive注册表栈位置
     *
     * @see LuaCApi#_registerIndex()
     */
    final static String GLOBALS_INDEX = "LUA_REGISTRYINDEX";

    /**
     * 虚拟机native地址
     */
    final long L_State;
    /**
     * 标记正在调用的lua函数个数
     * @see LuaFunction#invoke
     */
    long calledFunction = 0;
    /**
     * 当前虚拟机加载的脚本状态、或执行脚本状态
     */
    private int state = JUST_INIT;
    /**
     * 错误日志
     */
    private String errorMsg = null;
    /**
     * 资源寻找器，Lua脚本调用require时需要
     * 优先级比{@link #resourceFinders}高
     *
     * @see #setResourceFinder(ResourceFinder)
     * @see #__onLuaRequire(long, String)
     * @see #onRequire(String)
     */
    private ResourceFinder resourceFinder;
    /**
     * 资源寻找器，Lua脚本调用require时需要
     *
     * 低优先
     *
     * @see #addResourceFinder(ResourceFinder)
     * @see #__onLuaRequire(long, String)
     * @see #onRequire(String)
     */
    private List<ResourceFinder> resourceFinders;
    /**
     * class -> luaClassName
     */
    private final HashMap<Class, String> luaClassNameMap;
    /**
     * 全局用户信息
     */
    private IGlobalsUserdata javaUserdata;
    /**
     * 此虚拟机是否可debug
     */
    private boolean debuggable;
    /**
     * 创建线程
     */
    private Thread mainThread;

    /**
     * 保存Native虚拟机指针和Java Globals表对应关系
     *
     * @see #createLState(boolean)
     * @see #getGlobalsByLState(long)
     */
    private static final HashMap<Long, Globals> cache = new HashMap<>();

    /**
     * @see #createLState(boolean)
     */
    private Globals(long L_State) {
        super(GLOBALS_INDEX);
        mainThread = Thread.currentThread();
        this.L_State = L_State;
        luaClassNameMap = new HashMap<>();
    }

    //<editor-fold desc="public Method">

    //<editor-fold desc="static Method">

    /**
     * 判断底层库是否初始化成功
     */
    public static boolean isInit() {
        if (!init) {
            try {
                is32bit = LuaCApi.is32bit();
                init = true;
            } catch (Throwable ignore) {
                init = false;
            }
        }
        return init;
    }

    /**
     * 判断平台是否是32位平台
     */
    public static boolean isIs32bit() {
        return is32bit;
    }

    /**
     * 判断文件是否被加密过
     */
    public static boolean isENFile(String p) {
        return LuaCApi._isSAESFile(p);
    }

    /**
     * 是否打开文件加密
     * 打开后，读取文件会判断是否是加密文件，写入文件会写入加密数据
     * 关闭后，读写文件都不使用加密
     * @param open false default
     */
    public static void openSAES(boolean open) {
        LuaCApi._openSAES(open);
    }

    /**
     * 创建Lua虚拟机，并返回Global表
     *
     * @return 返回Global表
     */
    public static Globals createLState(boolean debuggable) {
        long vmPointer = LuaCApi._createLState(debuggable);
        Globals g = new Globals(vmPointer);
        g.debuggable = debuggable;
        cache.put(vmPointer, g);
        return g;
    }

    /**
     * 通过Native虚拟机指针获取Java Globals表
     * 必须通过{@link #createLState(boolean)}创建的虚拟机才能返回对应的表
     *
     * @param state Native lua_State指针
     * @return Globals表
     * @see #createLState(boolean)
     */
    public static Globals getGlobalsByLState(long state) {
        return cache.get(state);
    }

    /**
     * 获取正在运行中的lua虚拟机数量
     */
    public static int getLuaVmSize() {
        return cache.size();
    }

    /**
     * 获取所有lua虚拟机使用的内存量，单位Byte
     * @return 如果编译时，没有打开J_API_INFO编译选项，则返回为0
     * @see #getLVMMemUse() 获取单个虚拟机的内存使用量
     */
    public static long getAllLVMMemUse() {
        return LuaCApi._allLvmMemUse();
    }

    /**
     * 获取native层全局java对象的个数
     * @return 如果编译时，没有打开J_API_INFO编译选项，则返回为0
     */
    public static long globalObjectSize() {
        return LuaCApi._globalObjectSize();
    }

    /**
     * 打印native层泄漏内存信息
     * 如果编译时，没有打开J_API_INFO编译选项，则不会有信息
     */
    public static void logMemoryLeakInfo() {
        LuaCApi._logMemoryInfo();
    }

    /**
     * 设置lua gc回调java gc时间间隔
     * 若小于等于0，表示关闭回调
     * @see #__onLuaGC(long)
     * @param offset <=0 mean close
     */
    public static void setGcOffset(int offset) {
        LuaCApi._setGcOffset(offset);
    }
    //</editor-fold>

    //<editor-fold desc="compile and execute">

    /**
     * 设置lua包根路径，require时使用
     * 优先级最高
     * @param basePath 根路径
     * @param autoSave 是否自动保存二进制文件
     *                 true：native查找到相关文件luab后缀是否可用，若可用，直接使用；
     *                       若不可用，查找lua相关文件是否可用，若可用，保存编译后二进制文件(luab)
     *                 false：native只查找已lua为后缀的相关文件
     */
    public void setBasePath(String basePath, boolean autoSave) {
        checkDestroy();
        basePath = basePath == null ? "" : basePath;
        LuaCApi._setBasePath(L_State, basePath, autoSave);
    }

    /*public boolean saveLoadedData(String chunkname, String path) {
        checkDestroy();
        try {
            return LuaCApi._saveChunk(L_State, path, chunkname) == LUA_OK;
        } catch (RuntimeException e) {
            e.printStackTrace();
            return false;
        }
    }*/

    /**
     * 将通过{@link #preloadData(String, byte[])} 或 {@link #preloadFile(String, String)}
     * 预加载的数据存储到文件中
     * @param chunkname 预加载时传入的名称
     * @param savePath  文件路径，必须保证父文件夹都存在
     * @return true 成功，false 为进行预加载，或存储出错，可能抛出异常
     */
    public boolean savePreloadData(String chunkname, String savePath) throws RuntimeException {
        checkDestroy();
        return LuaCApi._savePreloadData(L_State, savePath, chunkname) == LUA_OK;
    }

    /**
     * 打开调试
     * 需要在脚本未执行时打开，否则不生效
     * @param debug debug脚本
     * @param ip    ip地址
     * @param port  port
     * @return true：开启成功
     */
    public boolean startDebug(byte[] debug, String ip, int port) {
        checkDestroy();
        if (!debuggable) {
            throw new IllegalStateException("cannot start debug in a lvm without debug state.");
        }
        try {
            state = LuaCApi._startDebug(L_State, debug, ip, port);
        } catch (Throwable t) {
            errorMsg = t.getMessage();
            state = LUA_ERRINJAVA;
        }
        return state == LUA_OK;
    }

    /**
     * 加载lua源码字符串
     *
     * @param chunkName 名称
     * @param lua       Lua源码
     * @return 编译状态，true: 成功，可以通过{@link #callLoadedData()}执行
     * false: 失败，可通过{@link #getState()}获取加载状态
     */
    public boolean loadString(String chunkName, String lua) {
        return loadData(chunkName, lua.getBytes());
    }

    /**
     * 将lua源码编译成Lua二进制文件
     * 底层将自动保存文件，
     *
     * @param file 保存的文件地址
     * @param data lua源码
     * @return 是否成功
     * @note: 需要保证文件前的目录都存在
     */
    public boolean compileAndSave(String file, String chunkName, byte[] data) {
        checkDestroy();
        try {
            state = LuaCApi._compileAndSave(L_State, file, chunkName, data);
        } catch (Throwable t) {
            errorMsg = t.getMessage();
            state = LUA_ERRINJAVA;
        }

        return state == LUA_OK;
    }

    /**
     * 将lua源码编译成Lua二进制文件
     * 底层将自动保存文件，
     *
     * @param file 保存的文件地址
     * @param path lua源码路径
     * @return 是否成功
     * @note: 需要保证文件前的目录都存在
     */
    public boolean compileAndSave(String file, String path, String chunkName) {
        checkDestroy();
        try {
            state = LuaCApi._compilePathAndSave(L_State, file, path, chunkName);
        } catch (Throwable t) {
            errorMsg = t.getMessage();
            state = LUA_ERRINJAVA;
        }
        return state == LUA_OK;
    }

    /**
     * 加载Lua源码或二进制码
     * 二进制码必须由本机通过{@link #compileAndSave}编译
     * 其他机器编译出的二进制码不一定可用
     *
     * @param chunkName 名称
     * @param data      Lua源码或二进制码
     * @return 编译状态，true: 成功，可以通过{@link #callLoadedData()}执行
     * false: 失败，可通过{@link #getState()}获取加载状态
     */
    public boolean loadData(String chunkName, byte[] data) {
        checkDestroy();
        try {
            state = LuaCApi._loadData(L_State, chunkName, data);
        } catch (Throwable e) {
            e.printStackTrace();
            errorMsg = e.getMessage();
            state = LUA_ERRINJAVA;
        }
        return state == LUA_OK;
    }

    /**
     * 加载Lua源码或二进制码
     * 二进制码必须由本机通过{@link #compileAndSave}编译
     * 其他机器编译出的二进制码不一定可用
     *
     * @param path 脚本绝对路径
     * @return 编译状态，true: 成功，可以通过{@link #callLoadedData()}执行
     * false: 失败，可通过{@link #getState()}获取加载状态
     */
    public boolean loadFile(String path, String chunkName) {
        checkDestroy();
        try {
            state = LuaCApi._loadFile(L_State, path, chunkName);
        } catch (Throwable e) {
            e.printStackTrace();
            errorMsg = e.getMessage();
            state = LUA_ERRINJAVA;
        }
        return state == LUA_OK;
    }

    /**
     * 预加载Lua脚本
     * @param chunkName 脚本名称，Lua代码中require()时使用
     * @param data      源码或二进制码
     * @throws UndumpError 若编译出错，则抛出异常
     */
    public void preloadData(String chunkName, byte[] data) throws UndumpError {
        checkDestroy();
        LuaCApi._preloadData(L_State, chunkName, data);
    }

    /**
     * 预加载Lua文件
     * @param chunkName 脚本名称，Lua代码中require()时使用
     * @param path      脚本绝对路径
     * @throws UndumpError 若编译出错，则抛出异常
     */
    public void preloadFile(String chunkName, String path) throws UndumpError {
        checkDestroy();
        LuaCApi._preloadFile(L_State, chunkName, path);
    }

    /**
     * 设置主入口，必须已通过{@link #preloadData} 或 {@link #preloadFile} 预加载成功
     * @param chunkname 预加载时的米
     * @return true: 设置成功，可通过 {@link #callLoadedData()}执行
     */
    public boolean setMainEntryFromPreload(String chunkname) {
        checkDestroy();
        if (LuaCApi._setMainEntryFromPreload(L_State, chunkname)) {
            state = LUA_OK;
            return true;
        } else {
            state = -404;
            errorMsg = "Did not find " + chunkname + " module from _preload table";
        }
        return false;
    }

    /**
     * 若已通过
     *      {@link #loadString}
     *      {@link #loadData}
     *      {@link #compileAndSave}
     *      {@link #setMainEntryFromPreload}
     * 加载成功，可通过此方法执行加载脚本，并返回执行状态
     *
     * @return true: 成功，false: 失败，可通过{@link #getState()}查看执行状态
     */
    public boolean callLoadedData() {
        checkDestroy();
        if (state != LUA_OK) {
            if (state == JUST_INIT) {
                throw new IllegalStateException("Lua script is not loaded!");
            }
            throw new IllegalStateException("state of loading lua script is not ok, code: " + state);
        }
        try {
            state = LUA_CALLING;
            state = LuaCApi._doLoadedData(L_State);
        } catch (Throwable t) {
            t.printStackTrace();
            errorMsg = t.getMessage();
            state = LUA_ERRINJAVA;
        }
        return state == LUA_OK;
    }

    /**
     * 获取加载lua的状态，加载失败时使用
     *
     * @return 状态码
     * @see #loadString(String, String)
     * @see #loadData(String, byte[])
     */
    public int getState() {
        return state;
    }

    /**
     * 获取加载或执行Lua的错误信息
     * 当加载和执行状态码都为{@link #LUA_OK}时，信息为空
     *
     * @return 错误信息
     */
    public String getErrorMsg() {
        return errorMsg;
    }
    //</editor-fold>

    //<editor-fold desc="Register">

    /**
     * 注册静态Bridge
     * 在Lua中可通过 luaClassName:method()调用
     * <p>
     * 注意：
     * methodName 和 luaMethodName必须一一对应，且数组长度相等
     * pcs可为空， 或与methodName一一对应，且数组长度相等
     *
     * 必须和{@link #callLoadedData()}在同一线程！
     *
     * @param clz           java类
     * @param luaClassName  lua中调用的名称
     * @param methodName    方法
     * @param luaMethodName lua中调用的方法名
     * @param pcs           方法需要的参数个数
     *
     * use {@link #registerStaticBridgeSimple}
     */
    @Deprecated
    public void registerStaticBridge(String luaClassName, Class clz, String[] methodName, String[] luaMethodName, int[] pcs) {
        checkDestroy();
        LuaCApi._registerStaticClass(L_State, SignatureUtils.getClassName(clz), luaClassName, methodName, luaMethodName, pcs);
        luaClassNameMap.put(clz, luaClassName);
    }

    /**
     * 注册静态Bridge
     * 在Lua中可通过 luaClassName:method()调用
     *
     * 方法必须返回LuaValue数组，且参数必须为 (long, LuaValue[])
     *
     * 必须和{@link #callLoadedData()}在同一线程！
     *
     * @param clz          java类
     * @param luaClassName lua中调用的名称
     * @param methods      方法
     */
    public void registerStaticBridgeSimple(String luaClassName, Class clz, String... methods) {
        checkDestroy();
        LuaCApi._registerStaticClassSimple(L_State,
                SignatureUtils.getClassName(clz),
                luaClassName,
                findLuaParentClass(clz, luaClassNameMap),
                methods);
        luaClassNameMap.put(clz, luaClassName);
    }

    /**
     * 注册java userdata
     * 注意java构造函数一定需要有 Globals, LuaValue[] 参数
     * 方法的返回值和参数都必须是 LuaValue[]类型
     *
     * 若注册的class为{@link LuaUserdata}的子类，则会将对象保存到GNV表中，不可释放
     * 直到原生调用{@link LuaUserdata#destroy()}
     *
     * 注册后，马上设置元表，提升使用时性能
     *
     * 必须和{@link #callLoadedData()}在同一线程！
     *
     * @param luaClassName lua调用的类名
     * @param clz          java的class
     * @param methods      相关方法
     */
    public void registerUserdataSimple(String luaClassName, Class<? extends LuaUserdata> clz, String... methods) throws RuntimeException {
        checkDestroy();
        LuaCApi._registerUserdata(L_State,
                luaClassName,
                findLuaParentClass(clz, luaClassNameMap),
                SignatureUtils.getClassName(clz),
                methods);
        luaClassNameMap.put(clz, luaClassName);
    }

    /**
     * 注册所有的java userdata
     * 注意java构造函数一定需要有 Globals, LuaValue[] 参数
     * 方法的返回值和参数都必须是 LuaValue[]类型
     *
     * 若注册的class为{@link LuaUserdata}的子类，则会将对象保存到GNV表中，不可释放
     * 直到原生调用{@link LuaUserdata#destroy()}
     *
     * 注册后，马上设置元表，提升使用时性能
     *
     * 必须和{@link #callLoadedData()}在同一线程！
     * @param lcns          lua调用的类名
     * @param lpcns         继承自lua的类名
     * @param jcns          java的class {@link SignatureUtils#getClassName(Class)}
     * @param mc            每个userdata有多少个方法
     * @param lazy          每个userdata是否是lazy
     * @param allmethods    所有方法数组
     */
    public void registerAllUserdata(String[] lcns, String[] lpcns, String[] jcns, int[] mc, boolean[] lazy, String[] allmethods) throws RuntimeException {
        checkDestroy();
        final int len = lcns.length;
        if (len != lpcns.length || len != jcns.length || len != mc.length)
            throw new IllegalArgumentException("lcns lpcns jcns mc must have same length");
        LuaCApi._registerAllUserdata(L_State, lcns, lpcns, jcns, mc, lazy, allmethods);
    }

    /**
     * 注册java userdata
     * 注意java构造函数一定需要有 Globals, LuaValue[] 参数
     * 方法的返回值和参数都必须是 LuaValue[]类型
     *
     * 若注册的class为{@link LuaUserdata}的子类，则会将对象保存到GNV表中，不可释放
     * 直到原生调用{@link LuaUserdata#destroy()}
     *
     * 注册后，lua脚本未初始化此userdata时，将不会设置元表，减少内存和注册时间
     *
     * 必须和{@link #callLoadedData()}在同一线程！
     *
     * @param luaClassName  lua调用的类名
     * @param clz           java的class
     * @param methods       相关方法
     */
    public void registerLazyUserdataSimple(String luaClassName, Class<? extends LuaUserdata>  clz, String... methods) throws RuntimeException {
        checkDestroy();
        LuaCApi._registerUserdataLazy(L_State,
                luaClassName,
                findLuaParentClass(clz, luaClassNameMap),
                SignatureUtils.getClassName(clz),
                methods);
        luaClassNameMap.put(clz, luaClassName);
    }

    /**
     * 注册数字型枚举变量
     * @param lcn       lua中的名称
     * @param keys      枚举名称
     * @param values    数值
     */
    public void registerNumberEnum(String lcn, String[] keys, double[] values) {
        checkDestroy();
        if (keys == null || values == null)
            return;
        if (keys.length != values.length) {
            throw new IllegalArgumentException("keys and values must have same length!");
        }
        LuaCApi._registerNumberEnum(L_State, lcn, keys, values);
    }

    /**
     * 注册字符串型枚举变量
     * @param lcn       lua中的名称
     * @param keys      枚举名称
     * @param values    数值
     */
    public void registerStringEnum(String lcn, String[] keys, String[] values) {
        checkDestroy();
        if (keys == null || values == null)
            return;
        if (keys.length != values.length) {
            throw new IllegalArgumentException("keys and values must have same length!");
        }
        LuaCApi._registerStringEnum(L_State, lcn, keys, values);
    }
    //</editor-fold>

    //<editor-fold desc="Resource finder">
    /**
     * 设置资源寻找器
     *
     * 高优先
     *
     * @see #resourceFinder
     * @see ResourceFinder
     * @see #__onLuaRequire(long, String)
     * @see #onRequire(String)
     */
    public void setResourceFinder(ResourceFinder rf) {
        resourceFinder = rf;
    }

    /**
     * 添加资源寻找器
     *
     * 低优先
     *
     * @see #resourceFinders
     * @see ResourceFinder
     * @see #__onLuaRequire(long, String)
     * @see #onRequire(String)
     */
    public void addResourceFinder(ResourceFinder rf) {
        if (resourceFinders == null) {
            resourceFinders = new ArrayList<>();
        }
        if (!resourceFinders.contains(rf))
            resourceFinders.add(0, rf);
    }

    /**
     * 清除资源寻找器
     */
    public void clearResourceFinder() {
        if (resourceFinders != null)
            resourceFinders.clear();
    }
    //</editor-fold>

    /**
     * 获取当前虚拟机使用的内存量，单位Byte
     * @return 如果编译时，没有打开J_API_INFO编译选项，则返回为0
     * @see #getAllLVMMemUse() 获取所有虚拟机的内存使用量
     */
    public long getLVMMemUse() {
        return LuaCApi._lvmMemUse(L_State);
    }

    /**
     * dump出当前虚拟机堆栈
     * 测试SDK时使用
     *
     * @return 堆栈信息
     */
    public LuaValue[] dump() {
        checkDestroy();
        return LuaCApi._dumpStack(L_State);
    }

    /**
     * 销毁当前虚拟机
     */
    public void destroy() {
        if (!destroyed && (state == LUA_CALLING || calledFunction > 0)) {
            throw new IllegalStateException("throw in debug mode, cannot destroy lua vm when lua function is calling!");
        }
        synchronized (this) {
            if (destroyed)
                return;
            destroyed = true;
        }
        LuaCApi._close(L_State);
        if (javaUserdata != null) {
            javaUserdata.onGlobalsDestroy(this);
        }
        javaUserdata = null;
        NativeLog.release(L_State);
        cache.remove(L_State);
        luaClassNameMap.clear();
        resourceFinder = null;
        if (resourceFinders != null)
            resourceFinders.clear();
    }

    /**
     * 创建一个userdata，并设置到Global表里
     * luaClassName必须已经注册过
     *
     * @param key          键名称
     * @param luaClassName lua类名
     * @return 创建的对象
     */
    public Object createUserdataAndSet(String key, String luaClassName, LuaValue... params) throws RuntimeException {
        checkDestroy();
        return LuaCApi._createUserdataAndSet(L_State, key, luaClassName, params);
    }

    /**
     * 获取虚拟机地址
     */
    public long getL_State() {
        return L_State;
    }

    /**
     * 设置Java环境的用户数据
     */
    public void setJavaUserdata(IGlobalsUserdata userdata) {
        this.javaUserdata = userdata;
        NativeLog.register(L_State, userdata);
    }

    /**
     * 获取java环境的用户数据
     */
    public IGlobalsUserdata getJavaUserdata() {
        return javaUserdata;
    }

    /**
     * 通过class获取在lua中的名称
     * @param c clz
     * @return may be null
     *
     * @see LuaUserdata
     */
    public String getLuaClassName(Class c) {
        return luaClassNameMap.get(c);
    }

    /**
     * 设置class和luaclassname的对应关系
     */
    public void putLuaClassName(Map<Class, String> other) {
        luaClassNameMap.putAll(other);
    }

    /**
     * 查找class对象或其父class对象的注册信息
     * @param c class对象
     * @return 注册到Lua的类名
     */
    public static String findLuaParentClass(Class c, Map<Class, String> luaClassNameMap) {
        Class p = c;
        while (p != Object.class && p != JavaUserdata.class && p != LuaUserdata.class) {
            String s = luaClassNameMap.get(p);
            if (s != null)
                return s;
            p = p.getSuperclass();
        }
        return null;
    }
    //</editor-fold>

    //<editor-fold desc="Package Methods">

    /**
     * 判断当前线程是否是创建的线程
     */
    boolean isMainThread() {
        return mainThread == Thread.currentThread();
    }

    /**
     * 获取global表的位置
     * @see LuaUserdata
     */
    String globalsIndex() {
        return GLOBALS_INDEX;
    }

    /**
     * 在Lua栈上删除对应数据，如果此数据不是{@link NLuaValue}，或为Globals表，则无操作
     * 若数据不在当前Lua栈栈顶，则保留数据，直到下次触发删除数据
     *
     * @param value Lua数据
     * @return 是否真正将对应栈数据清除
     */
    boolean removeStack(final LuaValue value) {
        if (value instanceof Globals)
            return false;
        checkDestroy();
        final String key = value.nativeGlobalKey();
        if (key == null)
            return true;
        if (isMainThread())
            LuaCApi._removeNativeValue(L_State, key, value.type());
        else
            ;//TODO remove from other thread
        return true;
    }

    /**
     * 测试用
     * 弹出当前栈顶num个数据
     */
    void pop(int num) {
        checkDestroy();
        LuaCApi._pop(L_State, num);
    }

    /**
     * 测试用
     * 弹出当前栈顶数据
     */
    void pop() {
        checkDestroy();
        LuaCApi._pop(L_State, 1);
    }
    //</editor-fold>

    //<editor-fold desc="Table unsupported set get">
    public void set(int index, LuaValue value) {
        unsupported();
    }

    public void set(int index, double num) {
        unsupported();
    }

    public void set(int index, boolean b) {
        unsupported();
    }

    public void set(int index, String s) {
        unsupported();
    }

    @Override
    public LuaValue get(int index) {
        unsupported();
        return null;
    }

    private void unsupported() {
        throw new UnsupportedOperationException("global is not support set/get a number key!");
    }
    //</editor-fold>

    /**
     * Lua脚本调用require时
     *
     * @param L    Lua虚拟机地址
     * @param name require名称
     * @return null | absolutePath(LuaString) | lua byte data(LuaUserdata)
     * @see #onRequire
     */
    @LuaApiUsed
    private static Object __onLuaRequire(long L, String name) {
        return getGlobalsByLState(L).onRequire(name);
    }

    /**
     * Lua脚本执行GC时调用
     *
     * @see #setGcOffset(int)
     */
    @LuaApiUsed
    private static void __onLuaGC(long L) {
        System.gc();
    }

    /**
     * Lua脚本调用require时
     *
     * @param name require名称，一般不带后缀
     * @return null | absolutePath(LuaString) | lua byte data(LuaUserdata)
     * @see #__onLuaRequire
     */
    private Object onRequire(String name) {
        Object ret = findResource(resourceFinder, name);
        if (ret != null)
            return ret;
        if (resourceFinders != null) {
            for (ResourceFinder rf : resourceFinders) {
                ret = findResource(rf, name);
                if (ret != null) {
                    return ret;
                }
            }
        }
        return null;
    }

    private void checkDestroy() {
        if (destroyed) {
            throw new IllegalStateException("this lua vm is destroyed!");
        }
    }

    private static Object findResource(ResourceFinder rf, String name) {
        if (rf != null) {
            name = rf.preCompress(name);
            String path = rf.findPath(name);
            if (path != null) {
                return path;
            }
            byte[] data = rf.getContent(name);
            if (data != null) {
                rf.afterContentUse(name);
                return data;
            }
        }
        return null;
    }

    @Override
    public String toString() {
        return "Globals" + super.toString();
    }
}
