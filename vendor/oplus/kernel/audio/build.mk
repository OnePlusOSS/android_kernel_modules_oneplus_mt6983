MY_LOCAL_PATH := $(call my-dir)

# This makefile is only for DLKM
ifneq ($(findstring vendor,$(MY_LOCAL_PATH)),)

#This makefile is only for soft links
ifneq ($(findstring audio-kernel,$(MY_LOCAL_PATH)),)

DLKM_DIR := $(TOP)/device/qcom/common/dlkm

OPLUS_AUDIO_SRC_FILES := \
	$(wildcard $(MY_LOCAL_PATH)/*) \
	$(wildcard $(MY_LOCAL_PATH)/*/*) \
	$(wildcard $(MY_LOCAL_PATH)/*/*/*) \
	$(wildcard $(MY_LOCAL_PATH)/*/*/*/*) \
	$(wildcard $(MY_LOCAL_PATH)/*/*/*/*/*)

# Build/Package only in case of supported target
########################### Audio extend driver  ###########################
#Add for audio extend dirver
include $(CLEAR_VARS)
LOCAL_SRC_FILES           := $(OPLUS_AUDIO_SRC_FILES)
LOCAL_MODULE              := oplus_audio_extend.ko
LOCAL_MODULE_KBUILD_NAME  := oplus/qcom/oplus_audio_extend.ko
LOCAL_MODULE_TAGS         := optional
LOCAL_MODULE_DEBUG_ENABLE := true
LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
include $(DLKM_DIR)/Build_external_kernelmodule.mk
###########################################################

########################### TFA98xx-v6 CODEC  ###########################
#add for tfa98xx bringup
include $(CLEAR_VARS)
LOCAL_SRC_FILES           := $(OPLUS_AUDIO_SRC_FILES)
LOCAL_MODULE              := oplus_audio_tfa98xx_v6.ko
LOCAL_MODULE_KBUILD_NAME  := oplus/codecs/tfa98xx-v6/oplus_audio_tfa98xx_v6.ko
LOCAL_MODULE_TAGS         := optional
LOCAL_MODULE_DEBUG_ENABLE := true
LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
include $(DLKM_DIR)/Build_external_kernelmodule.mk
###########################################################

########################### AW882XX CODEC  ###########################
#add for aw882xx bringup
include $(CLEAR_VARS)
LOCAL_SRC_FILES           := $(OPLUS_AUDIO_SRC_FILES)
LOCAL_MODULE              := oplus_audio_aw882xx.ko
LOCAL_MODULE_KBUILD_NAME  := oplus/codecs/aw882xx/oplus_audio_aw882xx.ko
LOCAL_MODULE_TAGS         := optional
LOCAL_MODULE_DEBUG_ENABLE := true
LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
include $(DLKM_DIR)/Build_external_kernelmodule.mk
###########################################################

########################### SIPA  ###########################
#add for sia pa bringup
include $(CLEAR_VARS)
LOCAL_SRC_FILES           := $(OPLUS_AUDIO_SRC_FILES)
LOCAL_MODULE              := oplus_audio_sipa.ko
LOCAL_MODULE_KBUILD_NAME  := oplus/codecs/sipa/oplus_audio_sipa.ko
LOCAL_MODULE_TAGS         := optional
LOCAL_MODULE_DEBUG_ENABLE := true
LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
include $(DLKM_DIR)/Build_external_kernelmodule.mk
###########################################################

################# usbc_switch-symvers ##############
#ifdef OPLUS_ARCH_EXTENDS
include $(CLEAR_VARS)
LOCAL_SRC_FILES           := $(wildcard $(MY_LOCAL_PATH)/codecs/usbc_switch/*.c)
LOCAL_MODULE              := oplus-usbc_switch-symvers
LOCAL_MODULE_STEM         := Module.symvers
LOCAL_MODULE_KBUILD_NAME  := Module.symvers
LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
include $(DLKM_DIR)/Build_external_kernelmodule.mk
#endif /* OPLUS_ARCH_EXTENDS */
###########################################################

########################### usbc_switch  ###########################
#add for audio switch
include $(CLEAR_VARS)
LOCAL_SRC_FILES           := $(OPLUS_AUDIO_SRC_FILES)
LOCAL_MODULE              := oplus_usbc_switch.ko
LOCAL_MODULE_KBUILD_NAME  := oplus/codecs/usbc_switch/oplus_usbc_switch.ko
LOCAL_MODULE_TAGS         := optional
LOCAL_MODULE_DEBUG_ENABLE := true
LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
include $(DLKM_DIR)/Build_external_kernelmodule.mk
###########################################################

########################### SIPA TUNING  ###########################
#add for sia pa bringup
include $(CLEAR_VARS)
LOCAL_SRC_FILES           := $(OPLUS_AUDIO_SRC_FILES)
LOCAL_MODULE              := oplus_audio_sipa_tuning.ko
LOCAL_MODULE_KBUILD_NAME  := oplus/codecs/sipa/oplus_audio_sipa_tuning.ko
LOCAL_MODULE_TAGS         := optional
LOCAL_MODULE_DEBUG_ENABLE := true
LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
include $(DLKM_DIR)/Build_external_kernelmodule.mk
###########################################################

########################### AW87XXX  ###########################
#add for aw87xxx pa bringup
include $(CLEAR_VARS)
LOCAL_SRC_FILES           := $(OPLUS_AUDIO_SRC_FILES)
LOCAL_MODULE              := oplus_audio_aw87xxx.ko
LOCAL_MODULE_KBUILD_NAME  := oplus/codecs/aw87xxx/oplus_audio_aw87xxx.ko
LOCAL_MODULE_TAGS         := optional
LOCAL_MODULE_DEBUG_ENABLE := true
LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
include $(DLKM_DIR)/Build_external_kernelmodule.mk
###########################################################

########################### PA MANAGER  ###########################
#add for pa manager
include $(CLEAR_VARS)
LOCAL_SRC_FILES           := $(OPLUS_AUDIO_SRC_FILES)
LOCAL_MODULE              := oplus_audio_pa_manager.ko
LOCAL_MODULE_KBUILD_NAME  := oplus/qcom/oplus_speaker_manager/oplus_audio_pa_manager.ko
LOCAL_MODULE_TAGS         := optional
LOCAL_MODULE_DEBUG_ENABLE := true
LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
include $(DLKM_DIR)/Build_external_kernelmodule.mk
###########################################################
endif # audio-kernel
endif # DLKM check
