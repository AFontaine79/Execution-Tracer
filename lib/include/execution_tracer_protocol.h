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

#define TRACE_IDCODE_BUFFER_FULL        15      /**< Traced when the buffer fills before calling the log dump routine */

/**
 * These defines follow the style of SFR bitfield macros
 * in ST CMSIS device headers.
 */
#define TRACE_IDCODE_Pos                (28U)
#define TRACE_IDCODE_Msk                (0xFUL << TRACE_IDCODE_Pos)
#define TRACE_DATA_Pos                  (0U)
#define TRACE_DATA_Msk                  (0xFFFFFFF << TRACE_DATA_Pos)

#define TRACE_VERSION_V_Pos             (16U)
#define TRACE_VERSION_V_Msk             (0xFF << TRACE_VERSION_V_Pos)
#define TRACE_VERSION_V_Val             ('V')
#define TRACE_VERSION_MAJOR_Pos         (8U)
#define TRACE_VERSION_MAJOR_Msk         (0xFF << TRACE_VERSION_MAJOR_Pos)
#define TRACE_VERSION_MINOR_Pos         (0U)
#define TRACE_VERSION_MINOR_Msk         (0xFF << TRACE_VERSION_MINOR_Pos)

#define TRACE_FANDL_MODULE_Pos          (16U)
#define TRACE_FANDL_MODULE_Msk          (0xFFF << TRACE_FANDL_MODULE_Pos)
#define TRACE_FANDL_LINE_Pos            (0U)
#define TRACE_FANDL_LINE_Msk            (0xFFFF << TRACE_FANDL_LINE_Pos)

#endif /* LIB_INCLUDE_EXECUTION_TRACER_PROTOCOL_H_ */
