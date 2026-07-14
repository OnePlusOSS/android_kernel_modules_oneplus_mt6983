oplus_bsp_mm_osvelte-y += proc-memstat.o
oplus_bsp_mm_osvelte-y += logger.o
oplus_bsp_mm_osvelte-y += lowmem-dbg.o
oplus_bsp_mm_osvelte-y += sys-memstat.o
oplus_bsp_mm_osvelte-y += sys-dmabuf.o
oplus_bsp_mm_osvelte-y += mm-config.o
oplus_bsp_mm_osvelte-$(CONFIG_ASHMEM) += sys-ashmem.o
oplus_bsp_mm_osvelte-y += vsprintf-dup.o
obj-$(CONFIG_OPLUS_FEATURE_MM_OSVELTE) += oplus_bsp_mm_osvelte.o

oplus_bsp_mm_osvelte_dbg-y += proc-memstat.o
oplus_bsp_mm_osvelte_dbg-y += logger.o
oplus_bsp_mm_osvelte_dbg-y += lowmem-dbg.o
oplus_bsp_mm_osvelte_dbg-y += vsprintf-dup.o
oplus_bsp_mm_osvelte_dbg-y += sys-dmabuf.o
oplus_bsp_mm_osvelte_dbg-y += mm-config.o
oplus_bsp_mm_osvelte_dbg-$(CONFIG_ASHMEM) += sys-ashmem.o
oplus_bsp_mm_osvelte_dbg-y += sys-memstat-empty.o
obj-$(CONFIG_OPLUS_FEATURE_MM_OSVELTE_DBG) += oplus_bsp_mm_osvelte_dbg.o
ifeq ($(MEM_LIGHTOS), yes)
ccflags-y += -DOPLUS_FEATURE_MEM_4G_DEVICE
endif
