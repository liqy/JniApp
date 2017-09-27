package com.bwie.jniapp;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.TextView;

import java.util.Random;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("hello-jni");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        TextView textView = (TextView) findViewById(R.id.hello);
        textView.setText(stringFromJNI());
        accessField();
        textView.setText("");

        accessMethod(100);

        testChineseIn("我爱你中国");
        textView.setText(testChineseOut());
    }

    public native String stringFromJNI();

    //访问非静态属性str，修改它的值
    public String str = "Li lu";
    public native void accessField();


    //访问静态属性NUM，修改它的值
    public static int NUM = 1;
    public native void accessStaticField();

    //产生指定范围的随机数
    public int genRandomInt(int max){
        System.out.println("genRandomInt 执行了...max = "+ max);
        return new Random().nextInt(max);
    }

    public native void accessMethod(int max);

    public native void testChineseIn(String chinese);//传进去
    public native String testChineseOut();//取出来会乱码

}
