package com.xfy.luajava.utils;

import java.io.File;

/**
 * Created by Xiong.Fangyu on 2019/3/20
 *
 * 寻找存在的文件绝对路径
 */
public class PathResourceFinder implements ResourceFinder {
    private final String basePath;

    /**
     * 需要传入根目录
     */
    public PathResourceFinder(String basePath) {
        this.basePath = basePath;
    }

    @Override
    public String preCompress(String name) {
        if (name.endsWith(".lua"))
            name = name.substring(0, name.length() - 4);
        if (!name.contains(".."))
            return name.replaceAll("\\.", File.separator) + ".lua";
        return name + ".lua";
    }

    @Override
    public String findPath(String name) {
        File f = new File(basePath, name + "b");
        if (f.isFile())
            return f.getAbsolutePath();
        f = new File(basePath, name);
        if (f.isFile())
            return f.getAbsolutePath();
        return null;
    }

    @Override
    public byte[] getContent(String name) {
        return null;
    }

    @Override
    public void afterContentUse(String name) {

    }
}
