package com.xfy.luajava;

import org.junit.Assert;

/**
 * Created by Xiong.Fangyu on 2019-06-17
 */
class Utils {
    private static final String PATH = "/src/test/lib/libluajapi.so";

    static void loadLibrary() {
        final String SO_PATH = System.getProperty("user.dir") + PATH;
        System.load(SO_PATH);
    }

    static void onGlobalsDestroy() {
        long mem = Globals.getAllLVMMemUse();
        if (mem > 0) {
            Globals.logMemoryLeakInfo();
        }
        Assert.assertEquals(0, mem);
    }
}
