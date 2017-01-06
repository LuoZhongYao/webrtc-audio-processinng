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
LOCAL_MODULE := libwebrtc_apm
LOCAL_MODULE_TAGS := optional
LOCAL_CPP_EXTENSION := .cc
LOCAL_C_INCLUDES := $(TOP)/external
LOCAL_SRC_FILES := \
	pcm.cc\
    aec/aec_core.c\
    aec/aec_rdft.c\
    aec/aec_resampler.c\
    aec/echo_cancellation.c\
    aecm/aecm_core.c\
    aecm/echo_control_mobile.c\
    agc/agc.cc\
    agc/agc_manager_direct.cc\
    agc/histogram.cc\
    agc/legacy/analog_agc.c\
    agc/legacy/digital_agc.c\
    agc/utility.cc\
    audio_buffer.cc\
    audio_processing_impl.cc\
    beamformer/array_util.cc\
    beamformer/covariance_matrix_generator.cc\
    beamformer/nonlinear_beamformer.cc\
    echo_cancellation_impl.cc\
    echo_control_mobile_impl.cc\
    gain_control_impl.cc\
    intelligibility/intelligibility_enhancer.cc\
    intelligibility/intelligibility_utils.cc\
    level_estimator_impl.cc\
    logging/aec_logging_file_handling.cc\
    noise_suppression_impl.cc\
    processing_component.cc\
    rms_level.cc\
    splitting_filter.cc\
    three_band_filter_bank.cc\
    transient/moving_moments.cc\
    transient/transient_detector.cc\
    transient/transient_suppressor.cc\
    transient/wpd_node.cc\
    transient/wpd_tree.cc\
    typing_detection.cc\
    utility/delay_estimator.c\
    utility/delay_estimator_wrapper.c\
    vad/gmm.cc\
    vad/pitch_based_vad.cc\
    vad/pitch_internal.cc\
    vad/pole_zero_filter.cc\
    vad/standalone_vad.cc\
    vad/vad_audio_proc.cc\
    vad/vad_circular_buffer.cc\
    vad/voice_activity_detector.cc\
    voice_detection_impl.cc\
    ns/noise_suppression.c\
    ns/ns_core.c\
    aecm/aecm_core_c.c\
	aec/aec_core_neon.c\
	aec/aec_rdft_neon.c\
	aecm/aecm_core_neon.c\
	ns/nsx_core_neon.c\
    high_pass_filter_impl.cc\

# Flags passed to both C and C++ files.
LOCAL_CFLAGS := \
    $(MY_WEBRTC_COMMON_DEFS) \
	-mfpu=neon\
    '-DWEBRTC_NS_FLOAT'

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libdl \
	libtinyalsa\
    libstlport

ifndef NDK_ROOT
include external/stlport/libstlport.mk
endif
include $(BUILD_STATIC_LIBRARY)

# apm process test app

#include $(CLEAR_VARS)
#
#LOCAL_MODULE_TAGS := tests
#LOCAL_CPP_EXTENSION := .cc
#LOCAL_SRC_FILES:= \
#    $(call all-proto-files-under, .) \
#    test/process_test.cc
#
## Flags passed to both C and C++ files.
#LOCAL_CFLAGS := \
#    $(MY_WEBRTC_COMMON_DEFS)
#
#LOCAL_C_INCLUDES := \
#    $(LOCAL_PATH)/interface \
#    $(LOCAL_PATH)/../interface \
#    $(LOCAL_PATH)/../.. \
#    $(LOCAL_PATH)/../../system_wrappers/interface \
#    external/gtest/include
#
#LOCAL_STATIC_LIBRARIES := \
#    libgtest \
#    libprotobuf-cpp-2.3.0-lite
#
#LOCAL_SHARED_LIBRARIES := \
#    libutils \
#    libstlport \
#    libwebrtc_audio_preprocessing
#
#LOCAL_MODULE:= webrtc_apm_process_test
#
#ifdef NDK_ROOT
#include $(BUILD_EXECUTABLE)
#else
#include external/stlport/libstlport.mk
#include $(BUILD_NATIVE_TEST)
#endif
#
## apm unit test app
#
#include $(CLEAR_VARS)
#
#LOCAL_MODULE_TAGS := tests
#LOCAL_CPP_EXTENSION := .cc
#LOCAL_SRC_FILES:= \
#    $(call all-proto-files-under, test) \
#    test/unit_test.cc \
#    ../../../test/testsupport/fileutils.cc
#
## Flags passed to both C and C++ files.
#LOCAL_CFLAGS := \
#    $(MY_WEBRTC_COMMON_DEFS) \
#    '-DWEBRTC_APM_UNIT_TEST_FIXED_PROFILE'
#
#LOCAL_C_INCLUDES := \
#    $(LOCAL_PATH)/interface \
#    $(LOCAL_PATH)/../interface \
#    $(LOCAL_PATH)/../.. \
#    $(LOCAL_PATH)/../../../test \
#    $(LOCAL_PATH)/../../system_wrappers/interface \
#    $(LOCAL_PATH)/../../common_audio/signal_processing/include \
#    external/gtest/include \
#    external/protobuf/src
#
#LOCAL_STATIC_LIBRARIES := \
#    libgtest \
#    libprotobuf-cpp-2.3.0-lite
#
#LOCAL_SHARED_LIBRARIES := \
#    libstlport \
#    libwebrtc_audio_preprocessing
#
#LOCAL_MODULE:= webrtc_apm_unit_test
#
#ifdef NDK_ROOT
#include $(BUILD_EXECUTABLE)
#else
#include external/stlport/libstlport.mk
#include $(BUILD_NATIVE_TEST)
#endif
