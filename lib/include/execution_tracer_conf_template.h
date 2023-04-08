/*
 * execution_tracer_conf.h
 *
 *  Created on: Oct 22, 2022
 *      Author: Aaron Fontaine
 */

#ifndef LIB_INCLUDE_EXECUTION_TRACER_CONF_H_
#define LIB_INCLUDE_EXECUTION_TRACER_CONF_H_

/**
 * To save buffer space, function entry and exit traces are performed as
 * offsets from the start of flash.  FLASH_BASE should equal the start of
 * FLASH space on your system.
 */
#define FLASH_BASE      (@EXEC_TRACE_FLASH_BASE@)

/**
 * RAM_BASE should equal the start of RAM space on your system.
 * Note: Non-contiguous segments are not supported.
 */
#define RAM_BASE        (@EXEC_TRACE_RAM_BASE@)

/**
 * SFR_BASE should equal the start of the peripheral registers area on
 * your system.
 * Note: Non-contiguous segments are not supported.
 */
#define SFR_BASE        (@EXEC_TRACE_SFR_BASE@)

/**
 * Execution tracer settings.
 */
#define ON              1
#define OFF             0

/**
 * When enabled, execution tracing will be halted by the second call to
 * TRACE_Init() (i.e. when the system finds itself going through the startup
 * sequence again.)  Trace entries that were not captured prior to the reset
 * can still be traced using void DumpExecTraceLog().
 * REQUIRES USE_NOINIT_RAM_FOR_TRACING to be enabled.
 * NOT YET IMPLEMENTED.
 */
#define STOP_TRACING_AFTER_RESET        (@EXEC_TRACE_STOP_TRACING_AFTER_RESET@)

/**
 * Place the trace buffer and control structure in noinit RAM.  This allows
 * persistence through resets and may be necessary for capturing trace info
 * prior to unexpected resources (hard fault, watchdog, etc).
 */
#define USE_NOINIT_RAM_FOR_TRACING      (@EXEC_TRACE_USE_NOINIT@)

/**
 * Determines what to do if the trace buffer is full when TRACE macros are
 * called.
 * - If enabled, the oldest trace entry will be overwritten.
 * - If disabled, the requested trace will not be performed.
 * Regardless of this setting, DumpExecTraceLog() will indicate that the
 * buffer reached maximum capacity.  This should be taken as indication
 * that entries were dropped.
 */
#define ALLOW_OVERWRITE                 (@EXEC_TRACE_ALLOW_OVERWRITE@)

/**
 * The number of trace entries that can be held in the trace buffer at once.
 * MUST BE A POWER OF 2.
 */
#define BUFFER_LENGTH_IN_WORDS          (@EXEC_TRACE_NUM_TRACE_ENTRIES@)

#endif /* LIB_INCLUDE_EXECUTION_TRACER_CONF_H_ */
