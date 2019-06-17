package com.xfy.luajava;

/**
 * Created by Xiong.Fangyu on 2019-06-14
 */
public class Log {

    static void log(String s) {
        System.out.println(s);
    }

    static void logf(String s, Object... obj) {
        System.out.println(String.format(s, obj));
    }
}
