/*
 * execution_tracer_protocol.h
 *
 *  Created on: Oct 23, 2022
 *      Author: Aaron Fontaine
 */

#ifndef LIB_INCLUDE_EXECUTION_TRACER_PROTOCOL_H_
#define LIB_INCLUDE_EXECUTION_TRACER_PROTOCOL_H_

/**
 * Having the protocol version in the trace buffer allows backwards
 * compatibility for the analyzer even for breaking changes.
 */
#define TRACE_PROTOCOL_MAJOR        1       /* Update for breaking changes */
#define TRACE_PROTOCOL_MINOR        0       /* Update for non-breaking changes */

/**
 * ID codes occupy the top 4 bits of each trace entry and identify
 * what type of entry it is and how it is to be decoded.
 */
#define TRACE_IDCODE_INVALID            0
#define TRACE_IDCODE_VERSION            1
#define TRACE_IDCODE_RESET              2
#define TRACE_IDCODE_FUNC_ENTRY         3
#define TRACE_IDCODE_FUNC_EXIT          4
#define TRACE_IDCODE_FILE_AND_LINE	    5
#define TRACE_IDCODE_VARIABLE_VALUE	    6
#define TRACE_IDCODE_SFR_VALUE		    7


#endif /* LIB_INCLUDE_EXECUTION_TRACER_PROTOCOL_H_ */
