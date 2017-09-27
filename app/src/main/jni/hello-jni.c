//
// Created by liqy on 2017/9/27.
//


#include <jni.h>
#include <android/log.h>
#include <string.h>

#include "demo.h"

// Android log function wrappers
static const
char *kTAG = "hello-jniCallback";

#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, kTAG, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))


const char *ClassName = "com/bwie/jniapp/MainActivity";

jstring stringJNI(JNIEnv *env, jobject instance) {
    return (*env)->NewStringUTF(env, "HHH=好HPPP");
}

jint accessMethod(JNIEnv *env, jobject jobj,jint max){
    jclass clz = (*env)->GetObjectClass(env, jobj);
    //拿到方法的ID，最后一个参数是方法的签名
    jmethodID mid = (*env)->GetMethodID(env, clz, "genRandomInt", "(I)I");
    //调用该方法，最后一个是可变参数，就是调用该方法所传入的参数
    //套路是如果返回是：Call返回类型Method
    jint jInt = (*env)->CallIntMethod(env, jobj, mid, max);
    LOGI("output from C ： %d", jInt);
}

void accessStaticField(JNIEnv *env, jobject jobj){
    jclass clz = (*env)->GetObjectClass(env, jobj);
    jfieldID fid = (*env)->GetStaticFieldID(env, clz, "NUM", "I");
    jint jInt = (*env)->GetStaticIntField(env, clz, fid);
    jInt++;
    (*env)->SetStaticIntField(env, clz, fid, jInt);
}

void accessField(JNIEnv *env, jobject jobj){
    LOGI("测试");

    //通过对象拿到Class
    jclass clz = (*env)->GetObjectClass(env, jobj);
    //拿到对应属性的ID
    jfieldID fid = (*env)->GetFieldID(env, clz, "str", "Ljava/lang/String;");
    //通过属性ID拿到属性的值
    jstring jstr = (*env)->GetObjectField(env, jobj, fid);

    //通过Java字符串拿到C字符串，第三个参数是一个出参，用来告诉我们GetStringUTFChars内部是否复制了一份字符串
    //如果没有复制，那么出参为isCopy，这时候就不能修改字符串的值了，因为Java中常量池中的字符串是不允许修改的（但是jstr可以指向另外一个字符串）
    char* cstr = (*env)->GetStringUTFChars(env, jstr, NULL);
    //在C层修改这个属性的值
    char res[20] = "I love you : ";
    strcat(res, cstr);

    //重新生成Java的字符串，并且设置给对应的属性
    jstring jstr_new = (*env)->NewStringUTF(env, res);
    (*env)->SetObjectField(env, jobj, fid, jstr_new);

    //最后释放资源，通知垃圾回收器来回收
    //良好的习惯就是，每次GetStringUTFChars，结束的时候都有一个ReleaseStringUTFChars与之呼应
    (*env)->ReleaseStringUTFChars(env, jstr, cstr);

}

jstring testChineseOut(JNIEnv *env, jobject jobj){

    char* c_str = "我爱你";
    jstring j_str = (*env)->NewStringUTF(env, c_str);
    return j_str;
////需要返回的字符串
//    char* c_str = "我爱你";
//    //jstring j_str = (*env)->NewStringUTF(env, c_str);
//
//    //通过调用构造方法String string = new String(byte[], charsetName);来解决乱码问题
//
//    //0.找到String类
//    jclass clz_String =  (*env)->FindClass(env, "java/lang/String");
//    jmethodID mid = (*env)->GetMethodID(env, clz_String, "<init>", "([BLjava/lang/String;)V");
//
//    //准备new String的参数：byte数组以及字符集
//    //1.创建字节数组，并且将C的字符串拷贝进去
//    jbyteArray j_byteArray = (*env)->NewByteArray(env, strlen(c_str));
//    (*env)->SetByteArrayRegion(env, j_byteArray, 0, strlen(c_str), c_str);
//    //2.创建字符集的参数，这里用Windows的more字符集GB2312
//    jstring charsetName = (*env)->NewStringUTF(env, "GB2312");
//
//    //调用
//    jstring j_new_str = (*env)->NewObject(env, clz_String, mid, j_byteArray, charsetName);
//    return j_new_str;
}

void testChineseIn(JNIEnv *env, jobject jobj,jstring chinese){
    char* c_chinese = (*env)->GetStringUTFChars(env, chinese, NULL);
    LOGI("%s", c_chinese);
}

//region 动态注册方法
/****
 * 声明需要动态注册的方法
 * typedef struct {
    const char* name;//Java方法的名字
    const char* signature;//Java方法的签名信息
    void*       fnPtr;//JNI中对应的方法指针
} JNINativeMethod;
 */
static JNINativeMethod gMethods[] = {
        {"stringFromJNI", "()Ljava/lang/String;",(void *)stringJNI},//对应java中的public native void verifySign();
        {"accessField", "()V",(void *)accessField},//对应java中的public native void verifySign();
        {"accessStaticField", "()V",(void *)accessStaticField},//对应java中的public native void verifySign();
        {"testChineseOut", "()Ljava/lang/String;",(void *)testChineseOut},//对应java中的public native void verifySign();
        {"testChineseIn", "(Ljava/lang/String;)V",(void *)testChineseIn},//对应java中的public native void verifySign();
        {"accessMethod", "(I)V",(void *)accessMethod}//对应java中的public native void verifySign();
};

/****
 * 注册方法
 */
static int registerNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *gMethods,
                                 int numMethods) {
    jclass clazz;
    clazz = (*env)->FindClass(env, className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    if ((*env)->RegisterNatives(env, clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

/****
 * 注册类
 */
static int registerNatives(JNIEnv *env) {
    return registerNativeMethods(env, ClassName, gMethods, sizeof(gMethods) / sizeof(gMethods[0]));
}
// endregion

//region  初始化默认函数
/****
 * 默认函数
 * 在调用 System.loadLibrary 时会调用，不需要手动调用
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    jint result = -1;

    if ((*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }
    if (!registerNatives(env)) {//注册
        return -1;
    }
    //成功
    result = JNI_VERSION_1_4;
    return result;
}

