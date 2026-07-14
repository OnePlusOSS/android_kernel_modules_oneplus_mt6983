#include <linux/types.h>
#include "platform.h"

/*to check platform, p should be "mtk" or "qcom"*/
bool platform_of(const char *p)
{
	return false;
}

bool ftm_mode(void)
{
	return false;
}

bool rf_mode(void)
{
	return false;
}

