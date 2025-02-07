# Build audio kernel driver
ifeq ($(call is-board-platform-in-list,kalama), true)
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/oplus_audio_extend.ko
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/oplus_audio_tfa98xx_v6.ko
endif

ifeq ($(call is-board-platform-in-list,pineapple), true)
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/oplus_audio_extend.ko
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/oplus_audio_tfa98xx_v6.ko
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/oplus_audio_sipa.ko
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/oplus_audio_sipa_tuning.ko
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/oplus_audio_aw882xx.ko
endif



ifeq ($(call is-board-platform-in-list,crow), true)
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/oplus_audio_extend.ko
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/oplus_audio_aw882xx.ko
endif

ifeq ($(call is-board-platform-in-list,blair), true)
# add analog pa
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/oplus_audio_sipa.ko
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/oplus_audio_sipa_tuning.ko
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/oplus_audio_aw87xxx.ko
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/oplus_audio_pa_manager.ko
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/oplus_usbc_switch.ko
endif # blair supported

ifeq ($(call is-board-platform-in-list,bengal), true)
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/oplus_audio_sipa.ko
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/oplus_audio_sipa_tuning.ko
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/oplus_audio_pa_manager.ko
endif # bengal supported

ifeq ($(call is-board-platform-in-list,sun), true)
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/oplus_audio_extend.ko
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/oplus_audio_aw882xx.ko
endif