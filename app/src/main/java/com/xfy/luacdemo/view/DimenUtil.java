package com.xfy.luacdemo.view;


import android.content.Context;
import android.view.ViewGroup;


/**
 * Dimension
 *
 * @author song
 */
public class DimenUtil {
    public static float sScale = -1;
    public static float spScale = -1;
    public static int WRAP_CONTENT;
    public static int MATCH_PARENT;
    
    public static void init(Context context) {
        sScale = context.getApplicationContext().getResources().getDisplayMetrics().density;
        spScale = context.getApplicationContext().getResources().getDisplayMetrics().scaledDensity;
        WRAP_CONTENT = (int) (ViewGroup.LayoutParams.MATCH_PARENT * sScale + 0.5f);
        MATCH_PARENT = (int) (ViewGroup.LayoutParams.MATCH_PARENT * sScale + 0.5f);
    }

    public static boolean isWrapContent(int scaleSize) {
        return scaleSize == WRAP_CONTENT;
    }

    public static boolean isMatchParent(int scaleSize) {
        return scaleSize == MATCH_PARENT;
    }

    public static int check(int scaleSize) {
        if (scaleSize == WRAP_CONTENT)
            return ViewGroup.LayoutParams.WRAP_CONTENT;
        if (scaleSize == MATCH_PARENT)
            return ViewGroup.LayoutParams.MATCH_PARENT;
        return scaleSize;
    }

    /**
     * convert dpi to px，返回给Android系统的必须是整数
     *
     * @param dpi
     * @return
     */
    public static int dpiToPx(double dpi) {
        return (int) (dpi * sScale);
    }

    /**
     * convert px to dpi ，返回给Lua层的调用，必须是浮点数
     *
     * @param px
     * @return
     */
    public static double pxToDpi(double px) {
        return px / sScale;
    }

    /**
     * 将sp值转换为px值，保证文字大小不变，给Android系统整数
     *
     * @param spValue（DisplayMetrics类中属性scaledDensity）
     * @return
     */
    public static int spToPx(double spValue) {
        return (int) (spValue * spScale);
    }

    /**
     * 将px转成sp值，返回给Lua层的调用，必须是浮点数
     *
     * @param pxValue
     * @return
     */
    public static double pxToSp(double pxValue) {
        return pxValue / spScale;
    }

}
