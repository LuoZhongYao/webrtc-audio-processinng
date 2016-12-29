# Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
#
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file in the root of the source
# tree. An additional intellectual property rights grant can be found
# in the file PATENTS.  All contributing project authors may
# be found in the AUTHORS file in the root of the source tree.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

include $(TOP)/external/webrtc/android-webrtc.mk

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libwebrtc_system_wrappers
LOCAL_MODULE_TAGS := optional
LOCAL_CPP_EXTENSION := .cc
LOCAL_SRC_FILES := \
    source/aligned_malloc.cc\
    source/clock.cc\
    source/condition_variable.cc\
    source/condition_variable_posix.cc\
    source/cpu_features.cc\
    source/cpu_info.cc\
    source/critical_section.cc\
    source/critical_section_posix.cc\
    source/data_log_c.cc\
    source/event.cc\
    source/event_timer_posix.cc\
    source/file_impl.cc\
    source/logging.cc\
    source/rtp_to_ntp.cc\
    source/rw_lock.cc\
    source/rw_lock_generic.cc\
    source/rw_lock_posix.cc\
    source/sleep.cc\
    source/sort.cc\
    source/tick_util.cc\
    source/timestamp_extrapolator.cc\
    source/trace_impl.cc\
    source/trace_posix.cc\
	source/logcat_trace_context.cc\
	source/atomic32_posix.cc\
    source/metrics_default.cc\
    source/data_log.cc\
	#source/cpu_features_android.c\

LOCAL_CFLAGS := \
    $(MY_WEBRTC_COMMON_DEFS)

LOCAL_C_INCLUDES := $(TOP)/external

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libdl \
    libstlport

#$(call import-module,android/cpufeatures)

ifndef NDK_ROOT
include external/stlport/libstlport.mk
endif
include $(BUILD_STATIC_LIBRARY)
