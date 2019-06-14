LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../lua
LOCAL_MODULE    := mlsocket
LOCAL_SRC_FILES := 	luasocket.c \
					buffer.c \
					auxiliar.c \
					options.c \
					timeout.c \
					io.c \
					usocket.c \
					compat.c \
					except.c \
					inet.c \
					mime.c \
					select.c \
					serial.c \
					tcp.c \
					udp.c

# LOCAL_STATIC_LIBRARIES := 	liblua

LOCAL_CFLAGS += -DLUA_DL_DLOPEN -DLUA_USE_C89 -DLUA_COMPAT_5_1 -DLUA_COMPAT_5_2 -DLUA_USE_LINUX -DANDROID

LOCAL_CFLAGS += -pie -fPIE

include $(BUILD_STATIC_LIBRARY)
