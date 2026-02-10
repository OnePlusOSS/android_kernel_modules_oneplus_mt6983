#undef TRACE_SYSTEM
#define TRACE_SYSTEM hbp

#if !defined(__HBP_TRACE_H__) || defined(TRACE_HEADER_MULTI_READ)
#define __HBP_TRACE_H__

#include <linux/tracepoint.h>

#ifndef __TRACE_EVENT_TOUCH_HELPER__
#define __TRACE_EVENT_TOUCH_HELPER__

enum {
	TRACE_START,
	TRACE_END,
	TRACE_REACHED
};

TRACE_DEFINE_ENUM(TRACE_START);
TRACE_DEFINE_ENUM(TRACE_END);
TRACE_DEFINE_ENUM(TRACE_REACHED);

#endif

TRACE_EVENT(hbp,
	    TP_PROTO(int id, const char *string, int phase),

	    TP_ARGS(id,	string,	phase),

	    TP_STRUCT__entry(
		    __field(int, id)
		    __string(msg, string?string:"<emtpy>")
		    __field(int, phase)
	    ),

	    TP_fast_assign(
		    __entry->id = id;
		    __assign_str(msg, string);
		    __entry->phase = phase;
	    ),

	    TP_printk("hbp_trace: id-%d %s %s",
		      __entry->id,
		      __get_str(msg),
		      __print_symbolic(__entry->phase,
{TRACE_START, "in"},
{TRACE_END, "out"},
{TRACE_REACHED, "reached"}))
	   );


#endif  /*__HBP_TRACE_H__*/

#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH .

#undef TRACE_INCLUDE_FILE
#define TRACE_INCLUDE_FILE hbp_trace

#include <trace/define_trace.h>