package com.xfy.luacdemo.view;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;

import com.xfy.luacdemo.App;
import com.xfy.luajava.Globals;
import com.xfy.luajava.wrapper.Register;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 * Created by Xiong.Fangyu on 2019/3/11
 */
public class LuaViewActivity extends Activity {
    private static final String TAG = "LuaViewActivity";
    private Globals globals;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        long start, end;
        DimenUtil.init(this);
        LuaViewManager viewManager = new LuaViewManager(this);

        start = now();
        globals = Globals.createLState(true);
        Register.install(globals);
//        globals.preloadData("test", readAssetsTxt(this, "test.lua"));
//        String path = App.getSDCardPath() + "test.lua";
//        globals.savePreloadData("test", path);
//        String path = App.getSDCardPath() + "compiled.lua";
//        globals.compileAndSave(path, readAssetsTxt(this, "tableView.lua"));
//        Log.d(TAG, "dump: " + Arrays.toString(globals.dump()));
        end = now();
        Log.d(TAG, "init globals cast " + cast(start, end));
        globals.setJavaUserdata(viewManager);

        start = now();
        UDViewGroup window = (UDViewGroup) globals.createUserdataAndSet("window", "View");
        setContentView(window.getView());
        end = now();
        Log.d(TAG, "set content view cast " + cast(start, end));

        new Thread(new Runnable() {
            @Override
            public void run() {
                long start, end;
                start = now();
                byte[] data = readAssetsTxt(LuaViewActivity.this, "luaview.lua");
                end = now();
                Log.d(TAG, "read cast " + cast(start, end));
                start = now();
                boolean r = globals.loadData("TestView", data);
                end = now();
                Log.d(TAG, "undump cast " + cast(start, end));

                if (r) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            long start, end;
                            start = now();
                            boolean r = globals.callLoadedData();
                            end = now();
                            Log.d(TAG, "call cast " + cast(start, end) + " success: " +  (r ? "success" : String.format("failed, code %d, msg: %s", globals.getState(), globals.getErrorMsg())));
                        }
                    });
                    return;
                }
                Log.e(TAG, String.format("undump failed, code %d, msg: %s", globals.getState(), globals.getErrorMsg()));
            }
        }).start();
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

    public static byte[] readSDCard(String name) {
        try {
            return is2byte(new FileInputStream(new File(App.getSDCardPath(), name)));
        } catch (Throwable e) {
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

    private static final long now() {
        return System.nanoTime();
    }

    private static String cast(long s, long e) {
        return String.format("%.2f ms", ((e - s) / 1000000f));
    }
}
