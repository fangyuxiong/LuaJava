package com.xfy.luacdemo;

import android.app.Application;
import android.os.Environment;

import com.xfy.luacdemo.view.GravityE;
import com.xfy.luacdemo.view.MBit;
import com.xfy.luacdemo.view.UDColor;
import com.xfy.luacdemo.view.UDViewGroup;
import com.xfy.luacdemo.wrapper.AreaUtils;
import com.xfy.luacdemo.wrapper.Size;
import com.xfy.luajava.wrapper.Register;
import com.xfy.luajava.wrapper.Translator;

import java.io.File;

/**
 * Created by Xiong.Fangyu on 2019/3/14
 */
public class App extends Application {

    private static App app;
    private String SD_CARD_PATH;
    @Override
    public void onCreate() {
        super.onCreate();
        app = this;
        init();

        Register.registerUserdata(Register.newUDHolder("Color", UDColor.class, true));
        Register.registerUserdata(Register.newUDHolder("View", UDViewGroup.class, true, UDViewGroup.methods));
        Register.registerStaticBridge("AreaUtils", AreaUtils.class);
        Register.registerStaticBridge(Register.newSHolder("MBit", MBit.class, "bor"));
        Register.registerEnum(GravityE.class);
        Register.registerUserdata(Size.class, false, "Size");
        Translator.registerL2JAuto(Size.class);
        Translator.registerJ2LAuto(Size.class);
    }

    public static App getApp() {
        return app;
    }

    public static String getSDCardPath() {
        return app.SD_CARD_PATH;
    }

    private void init() {
        try {
            SD_CARD_PATH = Environment.getExternalStorageDirectory().getAbsolutePath();
            if (!SD_CARD_PATH.endsWith("/")) {
                SD_CARD_PATH += "/";
            }
            SD_CARD_PATH += "LuaCTest/";
            new File(SD_CARD_PATH).mkdirs();
        }catch (Exception e){
        }
    }
}
