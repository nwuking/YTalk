package com.example.ytalk;

import android.app.Application;
import android.content.Context;
import android.content.res.Configuration;

import androidx.annotation.NonNull;

import com.example.ytalk.base.Logger;
import com.example.ytalk.db.ChatDb;
import com.lidroid.xutils.DbUtils;
import com.nostra13.universalimageloader.cache.memory.impl.LruMemoryCache;
import com.nostra13.universalimageloader.core.ImageLoader;
import com.nostra13.universalimageloader.core.ImageLoaderConfiguration;
import com.nostra13.universalimageloader.core.assist.QueueProcessingType;

import java.io.File;

/**
 * 自定义application，用于全局变量的定义
 */
public class MyApplication extends Application {
    public static String DEFAULT_APP_PATH = "/sdcard/YTalk";
    public static String DEFAULT_USERS_PATH = "/sdcard/YTalk/Users";
    public static String DEFAULT_LOGS_PATH = "/sdcard/YTalk/Logs";

    public static MyApplication instance;

    private static Context context;
    private ChatDb chatDb;                      // 操作聊天消息
    private DbUtils dbUtils;                    // 数据库操作方法，第三方

    @Override
    public void onCreate() { //
        super.onCreate();

        instance = this;

        context = getApplicationContext();
        chatDb = new ChatDb(this, "chatdb", null, 1);
        dbUtils = DbUtils.create(this);

        initImageLoader();

        // 创建YTalk根目录
        String path = DEFAULT_APP_PATH;
        File appDir = new File(path);
        if(!appDir.exists()) {
            appDir.mkdir();
        }

        // 创建users目录
        File usersDir = new File(DEFAULT_USERS_PATH);
        if(!usersDir.exists()) {
            usersDir.mkdir();
        }

        // 创建logs目录
        File logsDir = new File(DEFAULT_LOGS_PATH);
        if(!logsDir.exists()) {
            logsDir.mkdir();
        }

        Logger.Init(true);
        Logger.LogInfo("MyApplacation init completed");
    }

    private void initImageLoader() {
        ImageLoaderConfiguration config = new ImageLoaderConfiguration.Builder(
                getApplicationContext()).threadPoolSize(3)
                .threadPriority(Thread.NORM_PRIORITY-2)
                .tasksProcessingOrder(QueueProcessingType.FIFO)
                .denyCacheImageMultipleSizesInMemory()
                .memoryCache(new LruMemoryCache(2*1024*1024))
                .memoryCacheSize(2*1024*1024).memoryCacheSizePercentage(13)
                .diskCacheSize(50*1024*1024).diskCacheFileCount(100)
                .writeDebugLogs().build();
        ImageLoader.getInstance().init(config);
    }
}
