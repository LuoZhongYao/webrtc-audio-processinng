LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

include $(TOP)/external/webrtc/android-webrtc.mk

LOCAL_ARM_MODE 		:= arm
LOCAL_MODULE 		:= libwebrtc_base
LOCAL_MODULE_TAGS 	:= optional
LOCAL_CPP_EXTENSION := .cc
LOCAL_CFLAGS 		:= $(MY_WEBRTC_COMMON_DEFS)

LOCAL_C_INCLUDES 	:= $(TOP)/external

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libdl \
    libstlport

LOCAL_SRC_FILES := \
    bitbuffer.cc\
    buffer.cc\
    bufferqueue.cc\
    bytebuffer.cc\
    checks.cc\
    criticalsection.cc\
    event.cc\
    event_tracer.cc\
    exp_filter.cc\
    md5.cc\
    md5digest.cc\
    platform_file.cc\
    platform_thread.cc\
    stringencode.cc\
    stringutils.cc\
    systeminfo.cc\
    thread_checker_impl.cc\
    timeutils.cc\
    logging.cc\

ifndef NDK_ROOT
include external/stlport/libstlport.mk
endif
include $(BUILD_STATIC_LIBRARY)
