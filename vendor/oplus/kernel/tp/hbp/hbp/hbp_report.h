#ifndef __HBP_REPORT_H__
#define __HBP_REPORT_H__


#define TOUCH_MAX_FINGERS 10

enum hbp_data_type {
	HBP_REQUEST_SOURCE,
	HBP_REPORT_DATA,
};

enum hbp_source_type {
	HBP_POWER_AVDD,
	HBP_POWER_VDDI,
	HBP_POWER_RESET
};

enum hbp_report_type {
	HBP_REPORT_OBJS,
	HBP_REPORT_PEN,
	HBP_REPORT_DIFF,
	HBP_REPORT_RAW
};

enum touch_area {
	AREA_NOTOUCH,
	AREA_EDGE,
	AREA_CRITICAL,
	AREA_NORMAL,
	AREA_CORNER,
} ;

struct object_slot {
	uint16_t x;
	uint16_t y;
	uint16_t z;
	uint8_t  width_major;
	uint8_t  touch_major;
	uint8_t  tx_press;
	uint8_t  rx_press;
	uint8_t  tx_er;
	uint8_t  rx_er;
	uint8_t  status;
	enum touch_area type;
};

struct object_info {
	/*fingers on touch*/
	uint8_t slots;
	struct object_slot obj[TOUCH_MAX_FINGERS];
};

#endif
