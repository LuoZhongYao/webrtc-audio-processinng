
# voice
include $(TOP)/external/webrtc/common_audio/Android.mk
include $(TOP)/external/webrtc/modules/audio_processing/Android.mk
include $(TOP)/external/webrtc/base/Android.mk
include $(TOP)/external/webrtc/system_wrappers/Android.mk

LOCAL_PATH 			:= $(call my-dir)
include $(CLEAR_VARS)

include $(TOP)/external/webrtc/android-webrtc.mk

LOCAL_ARM_MODE 		:= arm
LOCAL_MODULE 		:= libwapm
LOCAL_MODULE_TAGS 	:= optional
LOCAL_FORCE_STATIC_EXECUTABLE := true

LOCAL_WHOLE_STATIC_LIBRARIES := \
	libwebrtc_base\
    libwebrtc_apm \
    libwebrtc_apm_utility \
    libwebrtc_common_audio \
    libwebrtc_system_wrappers

# Add Neon libraries.
ifeq ($(WEBRTC_BUILD_NEON_LIBS),true)
LOCAL_WHOLE_STATIC_LIBRARIES += \
    libwebrtc_aecm_neon \
    libwebrtc_ns_neon
endif

LOCAL_STATIC_LIBRARIES := \
    libprotobuf-cpp-2.3.0-lite

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libdl \
    libstlport

LOCAL_PRELINK_MODULE := false
LOCAL_LDLIBS := -llog

ifndef NDK_ROOT
include external/stlport/libstlport.mk
endif
include $(BUILD_SHARED_LIBRARY)

