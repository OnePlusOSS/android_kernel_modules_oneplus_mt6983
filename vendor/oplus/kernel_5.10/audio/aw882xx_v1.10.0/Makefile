#for AWINIC AW882XX Smart PA
#MODULE_NAME = snd-soc-aw882xx

subdir-ccflags-y += -I$(srctree)/sound/soc/mediatek/common
subdir-ccflags-y += -I$(srctree)/sound/soc/mediatek/audio_dsp
subdir-ccflags-y += -I$(srctree)/sound/soc/mediatek/common
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/audio_ipi/include
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/include
subdir-ccflags-y += -I$(srctree)/drivers/base

# debugging support (also enables trace_printk)
ccflags-y += -Werror
ccflags-y += -DOPLUS_ARCH_EXTENDS
ccflags-y += $(call cc-option,-Wno-date-time,)

snd-soc-aw882xx-objs += aw882xx.o
snd-soc-aw882xx-objs += aw_monitor.o
snd-soc-aw882xx-objs += aw_init.o
snd-soc-aw882xx-objs += aw_dsp.o
snd-soc-aw882xx-objs += aw_device.o
snd-soc-aw882xx-objs += aw_calib.o
snd-soc-aw882xx-objs += aw_bin_parse.o
snd-soc-aw882xx-objs += aw_spin.o

obj-$(CONFIG_SND_SOC_CODEC_AW882XX) += snd-soc-aw882xx.o
