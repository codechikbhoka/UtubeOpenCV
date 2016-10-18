	LOCAL_PATH := $(call my-dir)

    include $(CLEAR_VARS)
	OPENCVROOT:= /home/gomchik-1404/Software/OpenCV-android-3.0.0-sdk
	OPENCV_CAMERA_MODULES:=on
	OPENCV_INSTALL_MODULES:=on
	OPENCV_LIB_TYPE:=SHARED
	include ${OPENCVROOT}/sdk/native/jni/OpenCV.mk

	LOCAL_MODULE := MyOpencvLibs
	LOCAL_SRC_FILES := libs/ColorHistogram.cpp libs/Fingertip.cpp libs/MixGaussian.cpp libs/HandOperations.cpp libs/OpencvNativeClass.cpp
	LOCAL_CFLAGS += -std=c++11
	LOCAL_LDLIBS += -llog -landroid
	include $(BUILD_SHARED_LIBRARY)