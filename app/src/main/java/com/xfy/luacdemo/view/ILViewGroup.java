package com.xfy.luacdemo.view;

import android.view.ViewGroup;


/**
 * Created by XiongFangyu on 2018/7/31.
 */
public interface ILViewGroup<U extends UDViewGroup> {

    void bringSubviewToFront(UDView child);

    void sendSubviewToBack(UDView child);

    ViewGroup.LayoutParams applyLayoutParams(ViewGroup.LayoutParams src, UDView.UDLayoutParams udLayoutParams);
}
