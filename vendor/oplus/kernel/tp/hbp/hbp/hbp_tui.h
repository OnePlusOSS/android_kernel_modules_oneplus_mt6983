#ifndef __HBP_TUI_H__
#define __HBP_TUI_H__

#if IS_ENABLED(CONFIG_GH_ARM64_DRV)

#include <linux/dma-mapping.h>
#include <linux/gunyah/gh_irq_lend.h>
#include <linux/gunyah/gh_mem_notifier.h>

enum hbp_env {
	ENV_NORMAL,
	ENV_PVM,
	ENV_TVM,
	ENV_INVAL,
};

struct vm_iomem {
	int32_t base;
	int32_t size;
};

struct vm_gpio_mem {
	int32_t base;
	int32_t size;
	int32_t offset;
};

struct hbp_vm_mem {
	const char *env_type;
	int iomem_size;
	struct vm_iomem *iomem;
	struct vm_gpio_mem reset_mem;
	struct vm_gpio_mem intr_mem;
};

struct hbp_vm_info {
	enum gh_irq_label irq_label;
	enum gh_mem_notifier_tag mem_tag;
	enum gh_vm_names vm_name;
	gh_memparcel_handle_t vm_mem_handle;
	void *mem_cookie;
	struct hbp_vm_mem *mem;
	atomic_t vm_state;
};

#endif

#endif /*__HBP_TUI_H__*/
