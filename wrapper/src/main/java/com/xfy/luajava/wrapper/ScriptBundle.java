package com.xfy.luajava.wrapper;

import com.xfy.luajava.Globals;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by Xiong.Fangyu on 2019/3/20
 *
 * 脚本包，可保存一个lua工程的所有文件
 */
public class ScriptBundle {

    /**
     * 根路径，不可为空
     */
    private final String basePath;
    /**
     * lua入口文件
     */
    private ScriptFile main;
    /**
     * 其他可能被require的文件
     */
    private Map<String, ScriptFile> children;

    /**
     * 构造方法
     * @param basePath 根路径
     */
    public ScriptBundle(String basePath) {
        this.basePath = basePath;
    }

    public String getBasePath() {
        return basePath;
    }

    public ScriptFile getMain() {
        return main;
    }

    public void setMain(ScriptFile main) {
        this.main = main;
    }

    public Map<String, ScriptFile> getChildren() {
        return children;
    }

    public int size() {
        return children != null ? children.size() : 0;
    }

    public ScriptFile getChild(String chunkname) {
        return children != null ? children.get(chunkname) : null;
    }

    public void addChild(ScriptFile c) {
        addChild(c.getChunkName(), c);
    }

    public void addChild(String chunkname, ScriptFile c) {
        if (children == null) {
            children = new HashMap<>();
        }
        children.put(chunkname, c);
    }

    public boolean hasChildren() {
        return children != null;
    }

    /**
     * 保存所有已预加载过的子文件
     * @param g 在此虚拟机中预加载
     */
    public void saveAllLoadedFile(Globals g) {
        if (children == null)
            return;
        for (Map.Entry<String, ScriptFile> e : children.entrySet()) {
            e.getValue().saveIfPreloaded(g, basePath);
        }
    }
}
