package com.example.ytalk.activities;

import android.os.Bundle;
import android.view.View;
import android.view.Window;

import androidx.appcompat.app.AppCompatActivity;

/**
 * 所有activity的父类
 */
public abstract class BaseActivity extends AppCompatActivity implements View.OnClickListener {
    @Override
    protected void onCreate(Bundle bundle) { // 所有的activity的启动都走这里初始化。
        super.onCreate(bundle);

        // 隐藏标题栏
        requestWindowFeature(Window.FEATURE_NO_TITLE);

        // 初始化application
    }
}
