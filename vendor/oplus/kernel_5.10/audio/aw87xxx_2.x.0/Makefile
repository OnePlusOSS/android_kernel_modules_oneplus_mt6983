#for AWINIC AW87XXX Smart K PA
#MODULE_NAME = snd-soc-aw87xxx

# debugging support (also enables trace_printk)
ccflags-y += -Werror
ccflags-y += -DOPLUS_ARCH_EXTENDS
ccflags-y += $(call cc-option,-Wno-date-time,)

snd-soc-aw87xxx-objs += aw87xxx_2_x_0.o
snd-soc-aw87xxx-objs += aw87xxx_device_2_x_0.o
snd-soc-aw87xxx-objs += aw87xxx_monitor_2_x_0.o
snd-soc-aw87xxx-objs += aw87xxx_bin_parse_2_x_0.o
snd-soc-aw87xxx-objs += aw87xxx_dsp_2_x_0.o
snd-soc-aw87xxx-objs += aw87xxx_acf_bin_2_x_0.o

obj-$(CONFIG_SND_SOC_AW87XXX_2_X_0) += snd-soc-aw87xxx.o
