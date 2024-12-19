#ifndef __PLATFORM_H__
#define __PLATFORM_H__

enum {
	HIGH_TEMP_DISABLED = 1,
};

extern bool platform_of(const char *p);
extern int32_t get_boot_mode(void);

#endif

