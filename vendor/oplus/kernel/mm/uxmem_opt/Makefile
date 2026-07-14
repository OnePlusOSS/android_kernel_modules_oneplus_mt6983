# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2018-2020 Oplus. All rights reserved.

LINUXINCLUDE += -I$(srctree)/

ifneq ($(MEM_LIGHTOS), yes)
obj-$(CONFIG_OPLUS_FEATURE_UXMEM_OPT) += oplus_bsp_uxmem_opt.o
else
ccflags-y += -DOPLUS_FEATURE_MEM_4G_DEVICE
endif

oplus_bsp_uxmem_opt-y := uxmem_opt.o

