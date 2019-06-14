package com.xfy.luajava;

import com.xfy.luajava.utils.LuaApiUsed;

/**
 * Created by Xiong.Fangyu on 2019/2/22
 * <p>
 * jni层类，不对外
 *
 * @see LuaValue
 * @see LuaTable
 * @see Globals
 * @see LuaFunction
 */
@LuaApiUsed
class LuaCApi {
    private static Boolean is32bit;

    static {
        try {
            System.loadLibrary("luajapi");
        } catch (Throwable ignore) {
        }
    }

    static boolean is32bit() {
        if (is32bit == null) {
            is32bit = _check32bit();
        }
        return is32bit;
    }

    private static native boolean _check32bit();

    static native long _lvmMemUse(long L);

    static native long _allLvmMemUse();

    static native long _globalObjectSize();

    static native void _logMemoryInfo();

    static native void _setGcOffset(int offset);

    //<editor-fold desc="saes">
    static native boolean _isSAESFile(String path);

    static native void _openSAES(boolean open);
    //</editor-fold>

    //<editor-fold desc="for Globals">
    static native int _compileAndSave(long L, String file, String chunkName, byte[] data);

    static native int _compilePathAndSave(long L, String file, String src, String chunkName);

    static native int _savePreloadData(long L, String savePath, String chunkname);

    static native int _saveChunk(long L, String savePath, String chunkname);

    static native long _createLState(boolean debug);

    static native void _setBasePath(long L, String basePath, boolean autoSave);

    static native void _close(long L_state);

    static native int _registerIndex();

    static native LuaValue[] _dumpStack(long L_state);

    static native void _removeStack(long L, int stackIndex);

    static native void _pop(long L, int num);

    static native int _getTop(long L);

    static native void _removeNativeValue(long L, String k, int type);
    //</editor-fold>

    //<editor-fold desc="load and execute">
    static native int _startDebug(long L, byte[] debug, String ip, int port);

    static native int _loadData(long L_state, String chunkName, byte[] data);

    static native int _loadFile(long L_state, String path, String chunkName);

    static native int _doLoadedData(long L_state);

    static native boolean _setMainEntryFromPreload(long L, String chunkname);

    static native void _preloadData(long L, String chunkName, byte[] data);

    static native void _preloadFile(long L, String chunkName, String path);
    //</editor-fold>

    //<editor-fold desc="Table api">
    static native String _createTable(long L);

    static native int _getTableSize(long L, String table);

    static native void _setTableNumber(long L, String table, int k, double n);

    static native void _setTableBoolean(long L, String table, int k, boolean v);

    static native void _setTableString(long L, String table, int k, String v);

    static native void _setTableNil(long L, String table, int k);

    static native void _setTableChild(long L, String table, int k, Object child, int type);

    static native void _setTableNumber(long L, String table, String k, double n);

    static native void _setTableBoolean(long L, String table, String k, boolean v);

    static native void _setTableString(long L, String table, String k, String v);

    static native void _setTableNil(long L, String table, String k);

    static native void _setTableChild(long L, String table, String k, Object child, int type);

    static native Object _getTableValue(long L, String table, int k);

    static native Object _getTableValue(long L, String table, String k);

    /**
     * Global使用，创建一个userdata，并加入到Global表里
     *
     * @param name    key名称
     * @param luaName userdata的名称
     * @return 对应userdata实例
     */
    static native Object _createUserdataAndSet(long L, String name, String luaName, LuaValue[] params);

    /**
     * 返回 {@link LuaTable.Entrys}
     */
    static native Object _getTableEntry(long L, String table);

    static native boolean _startTraverseTable(long L, String table);

    static native LuaValue[] _nextEntry(long L, boolean isGlobal);

    static native void _endTraverseTable(long L);
    //</editor-fold>

    //<editor-fold desc="function">
    static native LuaValue[] _invoke(long global, String gk, LuaValue[] params, int returnCount);

    static native String _registerStaticFunction(long global, String className, String methodName, int paramCount);

    static native void _registerStaticClassSimple(long L, String javaClassName, String luaClassName, String lpcn, String[] javaMethodName);

    static native void _registerStaticClass(long L, String jcn, String lcn, String[] jmn, String[] lmn, int[] pcs);

    static native void _registerUserdata(long L, String lcn, String lpcn, String jcn, String[] jmn);

    static native void _registerAllUserdata(long L, String[] lcns, String[] lpcns, String[] jcns, int[] mc, boolean[] lazy, String[] allmethods);

    static native void _registerUserdataLazy(long L, String lcn, String lpcn, String jcn, String[] jmn);

    static native void _registerNumberEnum(long L, String lcn, String[] keys, double[] values);

    static native void _registerStringEnum(long L, String lcn, String[] keys, String[] values);
    //</editor-fold>
}
