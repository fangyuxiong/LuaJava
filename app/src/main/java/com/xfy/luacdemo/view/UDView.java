package com.xfy.luacdemo.view;

import android.graphics.drawable.ColorDrawable;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;

import com.xfy.luajava.JavaUserdata;
import com.xfy.luajava.LuaFunction;
import com.xfy.luajava.LuaNumber;
import com.xfy.luajava.LuaUserdata;
import com.xfy.luajava.LuaValue;
import com.xfy.luajava.utils.LuaApiUsed;

/**
 * Created by Xiong.Fangyu on 2019/3/11
 */
public abstract class UDView<V extends View> extends JavaUserdata {
    private static final String TAG = "UDView";

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
    };
    /**
     * 必须有传入long和LuaValue[]的构造方法，且不可混淆
     *
     * @param L 虚拟机地址
     * @param v lua脚本传入的构造参数
     */
    public UDView(long L, LuaValue[] v) {
        super(L, v);
        view = newView((LuaViewManager) globals.getJavaUserdata());
    }

    protected V view;

    public final UDLayoutParams udLayoutParams = new UDLayoutParams();
    private LuaFunction clickCallback;
    
    protected abstract V newView(LuaViewManager luaViewManager);

    public V getView() {
        return view;
    }

    //<editor-fold desc="API">
    @LuaApiUsed
    public LuaValue[] width(LuaValue[] varargs) {
        if (varargs.length == 1) {
            int w = DimenUtil.dpiToPx(varargs[0].toDouble());
            setWidth(DimenUtil.check(w));
            return null;
        }
        return LuaValue.varargsOf(LuaNumber.valueOf(DimenUtil.pxToDpi(getWidth())));
    }

    protected void setWidth(float w) {
        final View view = getView();
        ViewGroup.LayoutParams params = view != null ? view.getLayoutParams() : null;
        if (params == null) {
            params = new ViewGroup.MarginLayoutParams((int) w, ViewGroup.LayoutParams.WRAP_CONTENT);
            view.setLayoutParams(params);
            return;
        }
        params.width = (int) w;
        view.setLayoutParams(params);
    }

    public int getWidth() {
        final View view = getView();
        if (view != null && view.getLayoutParams() != null) {
            return view.getLayoutParams().width >= 0 ? view.getLayoutParams().width : view.getWidth();
        }
        return view != null ? view.getWidth() : 0;
    }

    @LuaApiUsed
    public LuaValue[] height(LuaValue[] varargs) {
        if (varargs.length == 1) {
            setHeight(DimenUtil.dpiToPx(varargs[0].toDouble()));
            return null;
        }
        return LuaValue.varargsOf(LuaNumber.valueOf(DimenUtil.pxToDpi(getHeight())));
    }

    protected void setHeight(float h) {
        final View view = getView();
        ViewGroup.LayoutParams params = view != null ? view.getLayoutParams() : null;
        if (params == null) {
            params = new ViewGroup.MarginLayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, (int) h);
            view.setLayoutParams(params);
            return;
        }
        params.height = (int) h;
        view.setLayoutParams(params);
    }

    public int getHeight() {
        final View view = getView();
        if (view != null && view.getLayoutParams() != null) {
            return view.getLayoutParams().height >= 0 ? view.getLayoutParams().height : view.getHeight();
        }
        return view != null ? view.getHeight() : 0;
    }

    @LuaApiUsed
    public LuaValue[] marginLeft(LuaValue[] var) {
        if (var.length == 1) {
            udLayoutParams.realMarginLeft = DimenUtil.dpiToPx(var[0].toDouble());
            setRealMargins();
            getView().setTranslationX(0);
            return null;
        }
        return LuaValue.varargsOf(LuaNumber.valueOf(DimenUtil.pxToDpi(udLayoutParams.realMarginLeft)));
    }

    @LuaApiUsed
    public LuaValue[] marginTop(LuaValue[] var) {
        if (var.length == 1) {
            udLayoutParams.realMarginTop = DimenUtil.dpiToPx(var[0].toDouble());
            setRealMargins();
            getView().setTranslationY(0);
            return null;
        }
        return LuaValue.varargsOf(LuaNumber.valueOf(DimenUtil.pxToDpi(udLayoutParams.realMarginTop)));
    }


    @LuaApiUsed
    public LuaValue[] marginRight(LuaValue[] var) {
        if (var.length == 1) {
            udLayoutParams.realMarginRight = DimenUtil.dpiToPx(var[0].toDouble());
            setRealMargins();
            return null;
        }
        return LuaValue.varargsOf(LuaNumber.valueOf(DimenUtil.pxToDpi(udLayoutParams.realMarginRight)));
    }

    @LuaApiUsed
    public LuaValue[] marginBottom(LuaValue[] var) {
        if (var.length == 1) {
            udLayoutParams.realMarginBottom = DimenUtil.dpiToPx(var[0].toDouble());
            setRealMargins();
            return null;
        }
        return LuaValue.varargsOf(LuaNumber.valueOf(DimenUtil.pxToDpi(udLayoutParams.realMarginBottom)));
    }

    private void setRealMargins() {
        final View v = getView();
        ViewGroup.LayoutParams p = v.getLayoutParams();
        if (v.getParent() instanceof ILViewGroup) {
            p = ((ILViewGroup) v.getParent()).applyLayoutParams(p, udLayoutParams);
            v.setLayoutParams(p);
            return;
        }
        if (p == null) {
            p = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        }
        if (!(p instanceof ViewGroup.MarginLayoutParams)) {
            p = new ViewGroup.MarginLayoutParams(p);
        }
        ((ViewGroup.MarginLayoutParams) p).setMargins(udLayoutParams.realMarginLeft,
                udLayoutParams.realMarginTop, udLayoutParams.realMarginRight, udLayoutParams.realMarginBottom);
        v.setLayoutParams(p);
    }

    @LuaApiUsed
    public LuaValue[] setGravity(LuaValue[] var) {
        udLayoutParams.gravity = var[0].toInt();
        setRealMargins();
        return null;
    }

    @LuaApiUsed
    public LuaValue[] alpha(LuaValue[] v) {
        if (v.length == 1) {
            if (getView() != null) {
                getView().setAlpha((float) v[0].toDouble());
            }
            return null;
        }
        return LuaValue.varargsOf(LuaNumber.valueOf(getView().getAlpha()));
    }

    @LuaApiUsed
    public LuaValue[] bgColor(LuaValue[] var) {
        if (var.length == 1) {
            view.setBackgroundColor(((UDColor) var[0]).getColor());
            return null;
        }
        UDColor color = new UDColor(globals);
        color.color = ((ColorDrawable)view.getBackground()).getColor();

        return LuaValue.varargsOf(color);
    }

    @LuaApiUsed
    public LuaValue[] onClick(LuaValue[] var) {
        if (var.length == 1) {
            clickCallback = var[0].isFunction() ? var[0].toLuaFunction() : null;
            view.setOnClickListener(clickListener);
        }
        return null;
    }

    private View.OnClickListener clickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            if (clickCallback != null) {
                clickCallback.invoke(null);
            }
        }
    };

    //</editor-fold>

    public static class UDLayoutParams {
        /**
         * 这四个margin属性是给 普通容器使用的
         */
        public int marginLeft;
        public int marginTop;
        public int marginRight;
        public int marginBottom;
        /**
         * 给普通容器使用
         */
        public float centerX = Float.NaN, centerY = Float.NaN;
        /**
         * 这四个margin属性是给线性布局使用
         */
        public int realMarginLeft;
        public int realMarginTop;
        public int realMarginRight;
        public int realMarginBottom;
        /**
         * 重力给线性布局使用
         * @see #setRealMargins
         */
        public int gravity = Gravity.LEFT | Gravity.TOP;
        /**
         * 普通容器判断是否使用gravity和realmargin
         */
        public boolean useRealMargin = true;
        /**
         * 优先级
         */
        public int priority = 0;
    }

    @Override
    protected void __onLuaGc() {
        Log.d(TAG, "__onLuaGc: " + view);
    }
}
