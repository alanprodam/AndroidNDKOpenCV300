LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#opencv
OPENCVROOT:= /home/alantavares/OpenCV-Android/OpenCV-3.0.0-android-sdk
OPENCV_CAMERA_MODULES:=on
OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=SHARED
include ${OPENCVROOT}/sdk/native/jni/OpenCV.mk

LOCAL_SRC_FILES := com_unicamp_alantavares_monocularopencv300_OpencvNativeClass.cpp

LOCAL_LDLIBS     += -llog -ldl
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_MODULE := MyOpenCVLibs

include $(BUILD_SHARED_LIBRARY)