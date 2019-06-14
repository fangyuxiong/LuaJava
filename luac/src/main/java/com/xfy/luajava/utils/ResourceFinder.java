package com.xfy.luajava.utils;

/**
 * Created by Xiong.Fangyu on 2019/3/6
 * <p>
 * 资源寻找器，Lua脚本调用require时需要
 *
 * @see com.xfy.luajava.Globals#onRequire(String)
 */
public interface ResourceFinder {
    /**
     * 预处理模块名称
     * eg: return name.replaceAll("\\.", "/") + ".lua";
     *
     * @param name 名称中一般不带后缀，且文件夹用.表示
     *             eg: path.moduleA
     * @return 名称
     */
    String preCompress(String name);

    /**
     * 寻找文件绝对路径
     * 第一优先
     *
     * @param name 名称
     * @return null or 绝对路径
     */
    String findPath(String name);

    /**
     * 获取Lua脚本或二进制数据
     *
     * @param name 名称
     * @return null or 数据
     */
    byte[] getContent(String name);

    /**
     * 当使用完{@link #getContent(String)}数据后，回调
     * @param name 名称
     */
    void afterContentUse(String name);
}
