#ifndef __OPLUS_DEV_CHECK_H__
#define __OPLUS_DEV_CHECK_H__

int oplus_dev_check_init(void);
void oplus_dev_check_fini(void);
void oplus_tuning_dual_sta_support(struct nlattr *nla);

#endif