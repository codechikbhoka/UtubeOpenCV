	LOCAL_PATH := $(call my-dir)

	include $(CLEAR_VARS)
    LOCAL_MODULE := HandyAR-prebuilt
    LOCAL_SRC_FILES := $(LOCAL_PATH)/src/libHandyAR.a
    LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/src/main/include
    include $(PREBUILT_STATIC_LIBRARY)



    include $(CLEAR_VARS)
	OPENCVROOT:= /home/gomchik-1404/Software/OpenCV-android-3.0.0-sdk
	OPENCV_CAMERA_MODULES:=on
	OPENCV_INSTALL_MODULES:=on
	OPENCV_LIB_TYPE:=SHARED
	include ${OPENCVROOT}/sdk/native/jni/OpenCV.mk

	LOCAL_MODULE := MyOpencvLibs
	LOCAL_SRC_FILES := com_smis_utubeopencv_OpencvNativeClass.cpp
	LOCAL_LDLIBS += -llog
	LOCAL_STATIC_LIBRARIES := HandyAR-prebuilt
	include $(BUILD_SHARED_LIBRARY)