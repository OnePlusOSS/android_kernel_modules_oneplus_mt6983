# Build audio kernel driver
ifeq ($(call is-board-platform-in-list,kalama), true)
AUDIO_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/oplus_audio_extend.ko
AUDIO_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/oplus_audio_tfa98xx_v6.ko
endif

ifeq ($(call is-board-platform-in-list,pineapple), true)
AUDIO_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/oplus_audio_extend.ko
AUDIO_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/oplus_audio_tfa98xx_v6.ko
AUDIO_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/oplus_audio_sipa.ko
AUDIO_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/oplus_audio_sipa_tuning.ko
AUDIO_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/oplus_audio_aw882xx.ko
endif



ifeq ($(call is-board-platform-in-list,crow), true)
AUDIO_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/oplus_audio_extend.ko
AUDIO_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/oplus_audio_aw882xx.ko
endif

ifeq ($(call is-board-platform-in-list,blair), true)
# add analog pa
AUDIO_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/oplus_audio_sipa.ko
AUDIO_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/oplus_audio_sipa_tuning.ko
AUDIO_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/oplus_audio_aw87xxx.ko
AUDIO_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/oplus_audio_pa_manager.ko
AUDIO_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/oplus_usbc_switch.ko
endif # blair supported

ifeq ($(call is-board-platform-in-list,bengal), true)
AUDIO_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/oplus_audio_sipa.ko
AUDIO_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/oplus_audio_sipa_tuning.ko
AUDIO_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/oplus_audio_pa_manager.ko
endif # bengal supported

ifeq ($(call is-board-platform-in-list,sun), true)
AUDIO_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/oplus_audio_extend.ko
AUDIO_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/oplus_audio_aw882xx.ko
endif