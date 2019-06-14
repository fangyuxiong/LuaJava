package com.xfy.luacdemo;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.widget.TextView;

import com.xfy.luacdemo.view.LuaViewActivity;
import com.xfy.luacdemo.wrapper.TestWrapperActivity;
import com.xfy.luajava.Globals;

/**
 * Created by Xiong.Fangyu on 2019/3/1
 */
public class MainActivity extends Activity implements View.OnClickListener{
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);
        findViewById(R.id.test_lua).setOnClickListener(this);
        findViewById(R.id.luaview).setOnClickListener(this);
        Globals g = Globals.createLState(true);
        g.destroy();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.test_lua:
                startActivity(new Intent(MainActivity.this, TestWrapperActivity.class));
                break;
            case R.id.luaview:
                startActivity(new Intent(MainActivity.this, LuaViewActivity.class));
                break;
        }
    }
}
