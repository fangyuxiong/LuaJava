package com.xfy.luajava.wrapper;

import org.junit.Test;

import java.util.Arrays;

import static org.junit.Assert.*;

/**
 * Example local unit test, which will execute on the development machine (host).
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
public class ExampleUnitTest {
    @Test
    public void addition_isCorrect() {
        a();
        a(null);
        a("a");
        a("a", "b");
        a(new String[] {"a", "b"});
        a(1);
        a(1,2,3);
        a(new int[] {1, 2});
    }

    private void a(Object... p) {
        if (p == null) {
            l("null");
        } else {
            for (int i = 0; i < p.length; i ++) {
                Class c = p[i].getClass();
                l(i + " "+c + " "+isNum(c));
            }
        }
    }

    private void b(String[] b) {
        l(Arrays.toString(b));
    }

    private boolean isNum(Class c) {
        return Number.class.isAssignableFrom(c);
    }

    private static final void l(Object s) {
        System.out.println(String.valueOf(s));
    }
}