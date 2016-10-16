#include <android/asset_manager.h>
#include <cstring>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <jni.h>
#include <stdio.h>
#include <vector>

#ifndef _Included_com_smis_utubeopencv_OpencvNativeClass
#define _Included_com_smis_utubeopencv_OpencvNativeClass

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_convertGray
  (JNIEnv *, jclass, jlong, jlong);

JNIEXPORT jboolean JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_initialise(JNIEnv *, jclass, jstring absPath);

JNIEXPORT jint JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_getHandRegion
        (JNIEnv *, jclass, jlong, jlong);

JNIEXPORT jboolean JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_getLearningMode
        (JNIEnv *, jclass);

JNIEXPORT jint JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_setLearningMode
        (JNIEnv *, jclass, jboolean);

JNIEXPORT jstring JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_getFilterAlgo
        (JNIEnv *, jclass);

JNIEXPORT jint JNICALL Java_com_smis_utubeopencv_OpencvNativeClass_setFilterAlgo
        (JNIEnv *, jclass, jstring);

#ifdef __cplusplus
}
#endif

#endif
