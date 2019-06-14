package com.xfy.luacdemo.wrapper;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;

import com.xfy.luajava.Globals;
import com.xfy.luajava.LuaTable;
import com.xfy.luajava.utils.DisposableIterator;
import com.xfy.luajava.wrapper.Register;

import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;

/**
 * Created by Xiong.Fangyu on 2019/3/19
 */
public class TestWrapperActivity extends Activity {
    private Globals globals;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        globals = Globals.createLState(true);
        Register.install(globals);

        byte[] data = readAssetsTxt(this, "testwrapper.lua");
        if (globals.loadData("testwrapper", data)) {
            globals.callLoadedData();
        }



//        DisposableIterator<LuaTable.KV> iterator = globals.iterator();
//        if (iterator != null) {
//            while (iterator.hasNext()) {
//                LuaTable.KV kv = iterator.next();
//                Log.d("TestWrapperActivity", kv.toString());
//            }
//            iterator.dispose();
//        }
//        Log.d("TestWrapperActivity", "dump: " + Arrays.toString(globals.dump()));
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        globals.destroy();
    }

    public static byte[] readAssetsTxt(Context context, String fileName) {
        try {
            InputStream is = context.getAssets().open(fileName);
            return is2byte(is);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    private static byte[] is2byte(InputStream is) throws IOException {
        try {
            byte[] buffer = new byte[is.available()];
            is.read(buffer);
            return buffer;
        } finally {
            is.close();
        }
    }

}
