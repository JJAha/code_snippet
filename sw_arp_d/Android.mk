LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	arpping.c \
	get_hwaddr.c \
	net_status.c

LOCAL_MODULE := swarpd

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libcutils

LOCAL_MODULE_TAGS := eng

include $(BUILD_EXECUTABLE)

