package com.xfy.luajava.wrapper;

import android.content.Context;

import com.xfy.luajava.utils.ResourceFinder;

import java.io.InputStream;

/**
 * Created by Xiong.Fangyu on 2019/3/20
 *
 * 寻找在Android assets包下存在的文件数据
 */
public class AssetsResourceFinder implements ResourceFinder {
    private final Context context;

    /**
     * 需要传入上下文
     */
    public AssetsResourceFinder(Context context) {
        this.context = context.getApplicationContext();
    }

    @Override
    public String preCompress(String name) {
        return name.replaceAll("\\.", "/") + ".lua";
    }

    @Override
    public String findPath(String name) {
        return null;
    }

    @Override
    public byte[] getContent(String name) {
        try {
            InputStream is = context.getAssets().open(name);
            byte[] data = new byte[is.available()];
            if (is.read(data) == data.length)
                return data;
        } catch (Throwable e) {

        }
        return null;
    }

    @Override
    public void afterContentUse(String name) {

    }
}
