TOP 	:= $(call my-dir)
APP_PIE := true
APP_ABI	:= armeabi-v7a
APP_STL := gnustl_static
APP_CXXFLAGS := -std=c++11 -Wno-error=pointer-arith
APP_CONLYFLAGS := -std=c11
#NDK_TOOLCHAIN := arm-linux-androideabi-4.9
APP_ALLOW_MISSING_DEPS=true
