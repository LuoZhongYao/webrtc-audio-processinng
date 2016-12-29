LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

include $(TOP)/external/webrtc/android-webrtc.mk

LOCAL_ARM_MODE 		:= arm
LOCAL_MODULE_CLASS 	:= STATIC_LIBRARIES
LOCAL_MODULE 		:= libwebrtc_common_audio
LOCAL_MODULE_TAGS 	:= optional
LOCAL_CFLAGS 		:= $(MY_WEBRTC_COMMON_DEFS)
LOCAL_C_INCLUDES 	:= $(TOP)/external
LOCAL_SRC_FILES 	:= \
	../config.cc\
	../common_types.cc\
    audio_converter.cc\
    audio_ring_buffer.cc\
    audio_util.cc\
    blocker.cc\
    channel_buffer.cc\
    fft4g.c\
    fir_filter.cc\
    lapped_transform.cc\
    real_fourier.cc\
    real_fourier_ooura.cc\
    resampler/push_resampler.cc\
    resampler/push_sinc_resampler.cc\
    resampler/resampler.cc\
    resampler/sinc_resampler.cc\
    ring_buffer.c\
    signal_processing/auto_corr_to_refl_coef.c\
    signal_processing/auto_correlation.c\
    signal_processing/copy_set_operations.c\
    signal_processing/cross_correlation.c\
    signal_processing/division_operations.c\
    signal_processing/dot_product_with_scale.c\
    signal_processing/downsample_fast.c\
    signal_processing/energy.c\
    signal_processing/filter_ar.c\
    signal_processing/filter_ma_fast_q12.c\
    signal_processing/get_hanning_window.c\
    signal_processing/get_scaling_square.c\
    signal_processing/ilbc_specific_functions.c\
    signal_processing/levinson_durbin.c\
    signal_processing/lpc_to_refl_coef.c\
    signal_processing/min_max_operations.c\
    signal_processing/randomization_functions.c\
    signal_processing/real_fft.c\
    signal_processing/refl_coef_to_lpc.c\
    signal_processing/resample.c\
    signal_processing/resample_48khz.c\
    signal_processing/resample_by_2.c\
    signal_processing/resample_by_2_internal.c\
    signal_processing/resample_fractional.c\
    signal_processing/spl_init.c\
    signal_processing/spl_sqrt.c\
    signal_processing/splitting_filter.c\
    signal_processing/sqrt_of_one_minus_x_squared.c\
    signal_processing/vector_scaling_operations.c\
    sparse_fir_filter.cc\
    vad/vad.cc\
    vad/vad_core.c\
    vad/vad_filterbank.c\
    vad/vad_gmm.c\
    vad/vad_sp.c\
    vad/webrtc_vad.c\
    wav_file.cc\
    wav_header.cc\
    window_generator.cc\

LOCAL_SHARED_LIBRARIES := libstlport

ifeq ($(TARGET_ARCH),arm)
LOCAL_SRC_FILES += \
      signal_processing/complex_bit_reverse_arm.S\
      signal_processing/spl_sqrt_floor_arm.S\
      signal_processing/complex_fft.c

ifeq ($(APP_ABI),armeabi-v7a)
LOCAL_SRC_FILES +=\
      signal_processing/filter_ar_fast_q12_armv7.S
else
LOCAL_SRC_FILES +=\
      signal_processing/filter_ar_fast_q12.c
endif # APP_ABI armeabi-v7a

endif # TARGET_ARCH arm

ifeq ($(TARGET_OS)-$(TARGET_SIMULATOR),linux-true)
LOCAL_LDLIBS += -ldl -lpthread
endif

ifndef NDK_ROOT
include external/stlport/libstlport.mk
endif
include $(BUILD_STATIC_LIBRARY)
