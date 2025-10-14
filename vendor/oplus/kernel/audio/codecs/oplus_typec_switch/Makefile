# TypeC Switch object

ifeq ($(DEVICE_MODULES_PATH),)
    DEVICE_MODULES_PATH = $(srctree)
else
endif
ifneq ($(CONFIG_MTK_USB_TYPEC_MUX),)
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/typec/mux/
endif

ifndef CONFIG_OPLUS_PD_EXT_SUPPORT
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/typec/tcpc/inc
else
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/power/oplus/pd_ext/inc
endif

snd-soc-typec-switch-objs := oplus_typec_switch.o
obj-$(CONFIG_SND_SOC_OPLUS_TYPEC_SWITCH) += snd-soc-typec-switch.o
