package com.xfy.luacdemo.view;

import android.content.Context;
import android.util.Log;

import com.xfy.luajava.Globals;
import com.xfy.luajava.utils.IGlobalsUserdata;

/**
 * Created by Xiong.Fangyu on 2019/3/11
 */
public class LuaViewManager implements IGlobalsUserdata {

    public Context context;

    public LuaViewManager(Context c) {
        context = c;
    }

    @Override
    public void onGlobalsDestroy(Globals g) {

    }

    @Override
    public void l(long L, String tag, String log) {
        Log.d(tag, log);
    }

    @Override
    public void e(long L, String tag, String log) {
        Log.e(tag, log);
    }
}
