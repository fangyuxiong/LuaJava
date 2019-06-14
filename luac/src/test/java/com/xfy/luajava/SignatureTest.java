package com.xfy.luajava;

import com.xfy.luajava.utils.SignatureUtils;

import org.junit.Test;

import java.lang.reflect.Method;

import static org.junit.Assert.*;

/**
 * Example local unit test, which will execute on the development machine (host).
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
public class SignatureTest {
    @Test
    public void logClass() {
        log(String.valueOf(Integer[].class.getComponentType()));
    }

    @Test
    public void testCheckValid() {
        assertEquals(true, SignatureUtils.isValidClassType(int.class, true));
        assertEquals(true, SignatureUtils.isValidClassType(Integer.class, true));
        assertEquals(true, SignatureUtils.isValidClassType(LuaValue.class, true));
        assertEquals(true, SignatureUtils.isValidClassType(LuaNumber.class, true));
        assertEquals(false, SignatureUtils.isValidClassType(SignatureUtils.class, true));
        assertEquals(false, SignatureUtils.isValidClassType(int[].class, false));
        assertEquals(true, SignatureUtils.isValidClassType(Integer[].class, true));
        assertEquals(true, SignatureUtils.isValidClassType(LuaValue[].class, true));
        assertEquals(true, SignatureUtils.isValidClassType(LuaNumber[].class, true));
        assertEquals(false, SignatureUtils.isValidClassType(SignatureUtils[].class, true));
    }

    @Test
    public void testClassSignature() {
        assertEquals("Z", SignatureUtils.getClassSignature(boolean.class));
        assertEquals("B", SignatureUtils.getClassSignature(byte.class));
        assertEquals("C", SignatureUtils.getClassSignature(char.class));
        assertEquals("S", SignatureUtils.getClassSignature(short.class));
        assertEquals("I", SignatureUtils.getClassSignature(int.class));
        assertEquals("J", SignatureUtils.getClassSignature(long.class));
        assertEquals("F", SignatureUtils.getClassSignature(float.class));
        assertEquals("D", SignatureUtils.getClassSignature(double.class));
        assertEquals("V", SignatureUtils.getClassSignature(void.class));
        assertEquals("Ljava/lang/Object;", SignatureUtils.getClassSignature(Object.class));
        assertEquals("Ljava/lang/Void;", SignatureUtils.getClassSignature(Void.class));
        assertEquals("Ljava/lang/Integer;", SignatureUtils.getClassSignature(Integer.class));
        assertEquals("Ljava/lang/String;", SignatureUtils.getClassSignature(String.class));
        assertEquals("Lcom/xfy/luajava/LuaValue;", SignatureUtils.getClassSignature(LuaValue.class));

        assertEquals("[Z", SignatureUtils.getClassSignature(boolean[].class));
        assertEquals("[Lcom/xfy/luajava/LuaValue;", SignatureUtils.getClassSignature(LuaValue[].class));
    }

    @Test
    public void testMethodSignature() throws Exception{
        assertEquals("()V",
                SignatureUtils.getMethodSignature(SignatureTest.class.getDeclaredMethod("testClassSignature")));
        assertEquals("(Ljava/lang/String;)V",
                SignatureUtils.getMethodSignature(SignatureTest.class.getDeclaredMethod("log", String.class)));
        assertEquals("(Ljava/lang/reflect/Method;)Ljava/lang/String;",
                SignatureUtils.getMethodSignature(SignatureUtils.class.getDeclaredMethod("getMethodSignature", Method.class)));

        Class testClz = TestClass.class;
        Method testMethod = testClz.getDeclaredMethod("test1", int[].class);
        assertEquals("([I)V", SignatureUtils.getMethodSignature(testMethod));
        testMethod = testClz.getDeclaredMethod("test2", Integer[].class);
        assertEquals("([Ljava/lang/Integer;)V", SignatureUtils.getMethodSignature(testMethod));
        testMethod = testClz.getDeclaredMethod("test3", String[].class);
        assertEquals("([Ljava/lang/String;)V", SignatureUtils.getMethodSignature(testMethod));
        testMethod = testClz.getDeclaredMethod("test4", LuaValue[].class);
        assertEquals("([Lcom/xfy/luajava/LuaValue;)V", SignatureUtils.getMethodSignature(testMethod));

        testMethod = testClz.getDeclaredMethod("test5");
        assertEquals("()[B", SignatureUtils.getMethodSignature(testMethod));
        testMethod = testClz.getDeclaredMethod("test6");
        assertEquals("()[Ljava/lang/Character;", SignatureUtils.getMethodSignature(testMethod));
        testMethod = testClz.getDeclaredMethod("test7");
        assertEquals("()[Ljava/lang/Object;", SignatureUtils.getMethodSignature(testMethod));
        testMethod = testClz.getDeclaredMethod("test8");
        assertEquals("()[Lcom/xfy/luajava/LuaString;", SignatureUtils.getMethodSignature(testMethod));

        testMethod = testClz.getDeclaredMethod("test9", int.class, String.class, LuaValue[].class, byte[].class);
        assertEquals("(ILjava/lang/String;[Lcom/xfy/luajava/LuaValue;[B)[Lcom/xfy/luajava/LuaFunction;", SignatureUtils.getMethodSignature(testMethod));
    }

    private static void log(String s) {
        System.out.println(s);
    }

    private static final class TestClass {
        private void test1(int[] a) {}
        private void test2(Integer[] a){}
        private void test3(String[] a){}
        private void test4(LuaValue[] a) {}
        private byte[] test5(){return null;};
        private Character[] test6(){return null;};
        private Object[] test7(){return null;};
        private LuaString[] test8(){return null;};

        private LuaFunction[] test9(int a, String b, LuaValue[] c, byte[] d) {return null;}
    }
}