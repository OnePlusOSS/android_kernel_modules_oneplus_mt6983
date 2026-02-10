# SIA PA object

ifeq ($(DEVICE_MODULES_PATH),)
    DEVICE_MODULES_PATH = $(srctree)
else
endif


subdir-ccflags-y += -I$(DEVICE_MODULES_PATH)/sound/soc/mediatek/audio_dsp
subdir-ccflags-y += -I$(DEVICE_MODULES_PATH)/sound/soc/mediatek/common
subdir-ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/audio_ipi/include
subdir-ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/include
subdir-ccflags-y += -I$(srctree)/drivers/base

COMPILE_TO_MODULE = y
PLATFORM_TYPE = mtk
EXTRA_CFLAGS += $(call cc-option,-Wno-date-time,)

ifeq ($(PLATFORM_TYPE),mtk)
EXTRA_CFLAGS += -DOPLUS_FEATURE_SPEAKER_MUTE
endif


snd-soc-sipa-3-1-0-objs := \
		sipa.o \
		sipa_regmap.o \
		sipa_aux_dev_if.o \
		sipa_91xx.o \
		sipa_parameter.o

snd-soc-sipa-tuning-3-1-0-objs := sipa_tuning_misc.o

ifeq ($(PLATFORM_TYPE),qcom_elite)
snd-soc-sipa-tuning-3-1-0-objs += sipa_afe.o
endif

ifeq ($(PLATFORM_TYPE),mtk)
snd-soc-sipa-tuning-3-1-0-objs += sipa_tuning_if.o # sipa_tuning_if_dev.o # sia81xx_ipi.o
endif

ifeq ($(PLATFORM_TYPE),qcom_spf)
snd-soc-sipa-tuning-3-1-0-objs += sipa_tuning_if.o # sipa_tuning_if_dev.o # sia81xx_ipi.o
endif

ifeq ($(COMPILE_TO_MODULE),y)
EXTRA_CFLAGS += -DSIPA_COMPILE_TO_MODULE
obj-m += snd-soc-sipa-3-1-0.o
obj-m += snd-soc-sipa-tuning-3-1-0.o
else
obj-y += snd-soc-sipa-3-1-0.o
obj-y += snd-soc-sipa-tuning-3-1-0.o
endif

