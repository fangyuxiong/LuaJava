LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../lua
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../luasocket
LOCAL_MODULE     := luajapi
LOCAL_SRC_FILES  := stack.c \
					map.c \
					m_mem.c \
					saes.c \
					utils.c \
					jlog.c \
					debug_info.c \
					jinfo.c \
					cache.c \
					jbridge.c \
					jtable.c \
					juserdata.c \
					compiler.c \
					luajapi.c
LOCAL_STATIC_LIBRARIES := 	mlsocket \
							liblua

LOCAL_LDLIBS := -llog

LOCAL_CFLAGS += -DLUA_DL_DLOPEN -DLUA_USE_C89 -DLUA_COMPAT_5_1 -DLUA_COMPAT_ALL -DLUA_USE_LINUX
LOCAL_CFLAGS += -DP_ANDROID
LOCAL_CFLAGS += -DJ_API_INFO
#LOCAL_CFLAGS += -DMEM_INFO
#LOCAL_CFLAGS += -DSAVE_LUAVALUE
#LOCAL_CFLAGS += -DJGR_TABLE

include $(BUILD_SHARED_LIBRARY)
