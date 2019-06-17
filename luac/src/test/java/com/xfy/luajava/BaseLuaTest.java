package com.xfy.luajava;

import org.junit.After;
import org.junit.Before;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

/**
 * Created by Xiong.Fangyu on 2019-06-17
 */
public abstract class BaseLuaTest {
    protected Globals globals;

    @Before
    public void initPath() {
        Utils.loadLibrary();
        assertTrue(Globals.isInit());
        Log.log("Lua vm is " + (Globals.isIs32bit() ? "32" : "64") + " bits");
        globals = Globals.createLState(true);
        Log.log("---------------on Start---------------");
    }

    @After
    public void onDestroy() {
        Log.log("---------------onDestroy---------------");
        globals.destroy();
        Utils.onGlobalsDestroy();
    }

    protected void checkStackSize(int size) {
        assertEquals(size, globals.dump().length);
    }
}
