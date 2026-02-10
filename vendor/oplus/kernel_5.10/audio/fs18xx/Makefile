###
# Copyright (C) 2018 Fourier Semiconductor Inc. All rights reserved.
#
EXTRA_CFLAGS += -Wall -Werror

subdir-ccflags-y += -I$(srctree)/sound/soc/mediatek/common

snd-smartpa-fs18xx-objs := fsm_core.o \
fs1603.o \
fsm_i2c.o

obj-$(CONFIG_SND_SMARTPA_FS18XX) += snd-smartpa-fs18xx.o
