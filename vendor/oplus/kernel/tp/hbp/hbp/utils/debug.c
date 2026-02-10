#include <linux/module.h>
#include "debug.h"

int debug_level = LOG_LEVEL_INFO;
EXPORT_SYMBOL(debug_level);

void set_debug_level(int new_level)
{
	/*debug level should under DEFAULT_DEBUG_LEVEL(6)*/
	if (new_level > LOG_LEVEL_DEBUG) {
		debug_level = LOG_LEVEL_DEBUG;
	} else {
		debug_level = new_level;
	}
}

int get_debug_level(void)
{
	return debug_level;
}

/*Todo: set debug level from cmdline or boot sconfig*/

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("OPLUS HBP");
MODULE_AUTHOR("OPLUS.");
