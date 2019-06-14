package com.xfy.luacdemo.view;

import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import com.xfy.luajava.LuaValue;
import com.xfy.luajava.utils.LuaApiUsed;

/**
 * Created by Xiong.Fangyu on 2019/3/11
 */
public class UDViewGroup extends UDView<FrameLayout> implements ILViewGroup<UDViewGroup> {
    public static final String[] methods = {
            "width",
            "height",
            "marginLeft",
            "marginTop",
            "marginRight",
            "marginBottom",
            "setGravity",
            "alpha",
            "bgColor",
            "onClick",
            "addView",
            "removeAllSubviews",
    };
    /**
     * 必须有传入long和LuaValue[]的构造方法，且不可混淆
     *
     * @param L 虚拟机地址
     * @param v lua脚本传入的构造参数
     */
    public UDViewGroup(long L, LuaValue[] v) {
        super(L, v);
    }

    //<editor-fold desc="API">
    @LuaApiUsed
    public LuaValue[] addView(LuaValue[] var) {
        if (var.length == 1)
            insertView((UDView) var[0], -1);
        return null;
    }

    protected void insertView(UDView view, int index) {
        FrameLayout v = getView();
        if (v == null || view == null)
            return;
        View sub = view.getView();
        if (sub == null)
            return;
        ViewGroup.LayoutParams layoutParams =
                applyLayoutParams(sub.getLayoutParams(),
                        view.udLayoutParams);
        v.addView(sub, index, layoutParams);
    }

    @LuaApiUsed
    public LuaValue[] removeAllSubviews(LuaValue[] v) {
        getView().removeAllViews();
        return null;
    }
    //</editor-fold>

    @Override
    public void bringSubviewToFront(UDView child) {

    }

    @Override
    public void sendSubviewToBack(UDView child) {

    }

    @Override
    public ViewGroup.LayoutParams applyLayoutParams(ViewGroup.LayoutParams src, UDLayoutParams udLayoutParams) {
        int l, t, r, b, gravity;
        if (udLayoutParams.useRealMargin) {
            l = udLayoutParams.realMarginLeft;
            t = udLayoutParams.realMarginTop;
            r = udLayoutParams.realMarginRight;
            b = udLayoutParams.realMarginBottom;
            gravity = udLayoutParams.gravity;
        } else {
            l = udLayoutParams.marginLeft;
            t = udLayoutParams.marginTop;
            r = udLayoutParams.marginRight;
            b = udLayoutParams.marginBottom;
            gravity = Gravity.LEFT | Gravity.TOP;
        }
        FrameLayout.LayoutParams ret = parseLayoutParams(src);
        ret.setMargins(l, t, r, b);
        ret.gravity = gravity;
        return ret;
    }

    @Override
    protected FrameLayout newView(LuaViewManager luaViewManager) {
        return new FrameLayout(luaViewManager.context);
    }

    private FrameLayout.LayoutParams parseLayoutParams(ViewGroup.LayoutParams src) {
        if (src == null) {
            src = generateNewWrapContentLayoutParams();
        } else if (!(src instanceof FrameLayout.LayoutParams)) {
            if (src instanceof ViewGroup.MarginLayoutParams) {
                src = generateNewLayoutParams((ViewGroup.MarginLayoutParams) src);
            } else {
                src = generateNewLayoutParams(src);
            }
        }
        return (FrameLayout.LayoutParams) src;
    }

    protected ViewGroup.LayoutParams generateNewWrapContentLayoutParams() {
        return new FrameLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
    }

    protected ViewGroup.LayoutParams generateNewLayoutParams(ViewGroup.MarginLayoutParams src) {
        return new FrameLayout.LayoutParams(src);
    }

    protected ViewGroup.LayoutParams generateNewLayoutParams(ViewGroup.LayoutParams src) {
        return new FrameLayout.LayoutParams(src);
    }
}
