package com.xfy.luacdemo;

import android.content.Context;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.TextView;

import com.xfy.luajava.Globals;
import com.xfy.luajava.LuaString;
import com.xfy.luajava.LuaTable;
import com.xfy.luajava.LuaValue;
import com.xfy.luajava.utils.ResourceFinder;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;

public class TestLuaActivity extends AppCompatActivity {
    private static final String TAG = "Activity";

    private static final String TEST = "test.lua";
    private static final String COM = "luacom.lua";
    private TextView textView;
    private Globals globals;
    private String SD_CARD_PATH;
    private String compileFilePath;
    private volatile LuaValue v;

    private static final String[] ms = new String[] {
            "a","b","c","d","e","f"
    };

    private static final int[] pcs = new int[] {
            -1,-1,-1,-1,-1,-1
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        textView = new TextView(this);
        setContentView(textView);
        init();
        compileFilePath = SD_CARD_PATH + "compileTest.lua";

        byte[] data = readAssetsTxt(this, TEST);
        globals = Globals.createLState(true);
        globals.setResourceFinder(new ResourceFinder() {
            @Override
            public String preCompress(String name) {
                if (name.indexOf('.') >= 0) {
                    name = name.replaceAll("\\.", "/");
                }
                return name + ".lua";
            }

            @Override
            public String findPath(String name) {
                return null;
            }

            @Override
            public byte[] getContent(String name) {
                return readAssetsTxt(TestLuaActivity.this, name);
            }

            @Override
            public void afterContentUse(String name) {

            }
        });
        long now = System.nanoTime();
//        try {
//            LuaCCompiler.compileAndSave(globals, compileFilePath, data);
//        } catch (Throwable t) {
//            t.printStackTrace();
//        }
        long end = System.nanoTime();
        Log.d(TAG, "编译耗时: " + (end - now));
//        Log.d(TAG, "dump0: " + Arrays.toString(globals.dump()));

        now = System.nanoTime();
        globals.registerStaticBridgeSimple(
                "StaticClassTest",
                LuaStaticClassTest.class,
                ms);
//        globals.registerStaticBridgeSimple(
//                LuaStaticClassTest2.class,
//                "StaticClassTest",
//                ms);
        end = System.nanoTime();
        Log.d(TAG, "registerStaticBridge ud cast: " + (end - now));

        Log.d(TAG, "dump1: " + Arrays.toString(globals.dump()));

        now = System.nanoTime();
        globals.registerUserdataSimple("UDT", LUDT.class, "a");
        end = System.nanoTime();
        Log.d(TAG, "register UDT cast: " + (end - now));
        now = System.nanoTime();
        globals.registerLazyUserdataSimple("LazyUD", LazyUserdata.class, "a");
        end = System.nanoTime();
        Log.d(TAG, "register LazyUD cast: " + (end - now));
        now = System.nanoTime();

        globals.registerUserdataSimple("UDTE", LuaUserdataExtends.class, "a", "b", "c");

        Object o = globals.createUserdataAndSet("window", "UDT");
        end = System.nanoTime();
        Log.d(TAG, "registerStaticBridge cast: " + (end - now));
        Log.d(TAG, "dump2: " + Arrays.toString(globals.dump()));
        Log.d(TAG, "window: " + globals.get("window") + " o:  "+ o);

//        data = readFile(compileFilePath);
        long cast;
        now = System.nanoTime();
        if (!globals.loadData("test", data)) {
            end = System.nanoTime();
            textView.setText("load failed, "+ globals.getState() + " msg: " + globals.getErrorMsg() + " cast: " + getCast(end - now));
            return;
        }
        end = System.nanoTime();
        cast = end - now;
        now = System.nanoTime();
        if (!globals.callLoadedData()) {
            end = System.nanoTime();
            textView.setText("call failed, " + globals.getState() + " msg: " + globals.getErrorMsg() + " load: " + getCast(cast) + " exe: " + getCast(end- now));
            return;
        }
        end = System.nanoTime();
        textView.setText("compile cast: " + getCast(cast) + "ms. do lua cast: " + getCast(end - now) + "ms");
        Log.d(TAG, "dump last: " + Arrays.toString(globals.dump()));
        Log.d(TAG, "global: " + globals.newEntry());
        LuaTable table = LuaTable.create(globals);
        table.set(1, 1);
        table.set("1", "2");
        Log.d(TAG, "get table: " + table.get(1) + " " + table.get("1"));

//        LuaFunction lf = (LuaFunction) globals.get("f3");
//        try {
//            lf.invoke(new LuaValue[]{LuaValue.Nil()});
//        } catch (Throwable t) {
//            t.printStackTrace();
//        }
//
//        Log.d(TAG, "dump0: " + Arrays.toString(globals.dump()));
    }

    private static LuaValue[] test(LuaValue[] params) {
        Log.d(TAG, "in java test: " + Arrays.toString(params));
        return new LuaValue[]{LuaString.valueOf("give you a java string")};
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

    public static byte[] readFile(String name) {
        try {
            InputStream is = new FileInputStream(new File(name));
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

    private static String getCast(long ns) {
        return String.format("%.2f", (ns / 1000000f));
    }

    private void init() {
        try {
            SD_CARD_PATH = Environment.getExternalStorageDirectory().getAbsolutePath();
            if (!SD_CARD_PATH.endsWith("/")) {
                SD_CARD_PATH += "/";
            }
            SD_CARD_PATH += "LuaCTest/";
        }catch (Exception e){
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        globals.destroy();
    }
}
