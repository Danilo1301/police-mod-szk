LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CPP_EXTENSION := .cpp .cc

ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    LOCAL_MODULE := PoliceModSZK
else
    LOCAL_MODULE := PoliceModSZK_64
endif

rwildcard = $(foreach d,$(wildcard $(1)/*),$(call rwildcard,$(d),$(2)) $(filter $(subst *,%,$(2)),$(d)))

LOCAL_SRC_FILES := $(call rwildcard,mod,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,src,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,json,*.cpp)

LOCAL_CXXFLAGS += -O2 -DNDEBUG -std=c++17

LOCAL_LDLIBS += -llog

include $(BUILD_SHARED_LIBRARY)