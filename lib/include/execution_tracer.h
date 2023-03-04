/*
 * execution_tracer.h
 *
 *  Created on: Oct 22, 2022
 *      Author: Aaron Fontaine
 */

#ifndef LIB_INCLUDE_EXECUTION_TRACER_H_
#define LIB_INCLUDE_EXECUTION_TRACER_H_

#include <stdint.h>
#include <stdbool.h>

#include "execution_tracer_conf.h"
#include "execution_tracer_protocol.h"

/* Check whether BUFFER_LENGTH_IN_WORDS is a power of 2 is in .c file */
#define BUFFER_INDEX_MASK       (BUFFER_LENGTH_IN_WORDS - 1)
#define BUFFER_MAX_CAPACITY     (BUFFER_LENGTH_IN_WORDS - 1)

#define EXEC_TRACE_INIT_MAGIC   (0xA5B4C123)

/**
 * Convenience functions for checking buffer status.
 * Note: It is not necessary to call TRACE_IsEmpty() in your idle handler.
 * Simply checking whether TRACE_Get() returned 0 is equivalent.
 */
#define TRACE_IsEmpty()         (m_exec_trace.head == m_exec_trace.tail)
#define TRACE_IsFull()          (((m_exec_trace.head + 1) & BUFFER_INDEX_MASK) == m_exec_trace.tail)
#define TRACE_GetNumEntries()   ((m_exec_trace.head - m_exec_trace.tail) & BUFFER_INDEX_MASK)

/**
 * @brief       Reset the execution trace buffer to zero entries.
 */
#define TRACE_Clear()                                           \
    do {                                                        \
        m_exec_trace.head = 0;                                  \
        m_exec_trace.tail = 0;                                  \
    } while (0)

#if ALLOW_OVERWRITE
#define TRACE_Put(n)                                            \
    do {                                                        \
        if (TRACE_IsFull()) {                                   \
            m_exec_trace.tail++;                                \
            m_exec_trace.tail &= BUFFER_INDEX_MASK;             \
        }                                                       \
        m_exec_trace.trace_buffer[m_exec_trace.head++] = n;     \
        m_exec_trace.head &= BUFFER_INDEX_MASK;                 \
    } while (0)
#else
#define TRACE_Put(n)                                            \
    do {                                                        \
        if (!TRACE_IsFull()) {                                  \
            m_exec_trace.trace_buffer[m_exec_trace.head++] = n; \
            m_exec_trace.head &= BUFFER_INDEX_MASK;             \
        }                                                       \
    } while (0)
#endif

/**
 * @brief       Retrieve one entry from the execution trace buffer.
 *              Call this function periodically in a background or idle handler
 *              to feed execution trace data to your desired backend.
 * @return      Nonzero - The next trace value from the buffer.
 *              Zero - Invalid; the buffer is empty.
 */
#define TRACE_Get() TRACE_IsEmpty() ? 0 :                       \
    (m_exec_trace.last_value = m_exec_trace.trace_buffer[m_exec_trace.tail++],   \
    m_exec_trace.tail &= BUFFER_INDEX_MASK,                     \
    m_exec_trace.last_value)

/**
 * @brief       Traces the protocol version of the execution tracer
 *              Call this once during system startup.
 */
#define TRACE_ExecTracerVersion()   TRACE_Put(                                      \
    ((TRACE_IDCODE_VERSION << TRACE_IDCODE_Pos) & TRACE_IDCODE_Msk) |               \
    ((TRACE_VERSION_V_Val << TRACE_VERSION_V_Pos) & TRACE_VERSION_V_Msk) |          \
    ((TRACE_PROTOCOL_MAJOR << TRACE_VERSION_MAJOR_Pos) & TRACE_VERSION_MAJOR_Msk) | \
    ((TRACE_PROTOCOL_MINOR << TRACE_VERSION_MINOR_Pos) & TRACE_VERSION_MINOR_Msk))

/**
 * @brief       Traces information about the processor reset
 *              Call this once during system startup.
 * @param[in]   reset_reg - User defined, the lower 28 bits are available.
 *              This should be the contents of the processor's reset register
 *              (E.g. RCC->CSR).  Shift and mask the register value to include
 *              only the relevant bits and avoid writing bits 31 through 28.
 */
#define TRACE_ProcessorReset(reset_reg) TRACE_Put(                      \
    ((TRACE_IDCODE_RESET << TRACE_IDCODE_Pos) & TRACE_IDCODE_Msk) |     \
    ((reset_reg << TRACE_DATA_Pos) & TRACE_DATA_Msk))

/**
 * @brief       Trace function entry and exit
 *              For all functions you wish to trace:
 *              - Place TRACE_FunctionEntry() as the first line of the function
 *              - Place TRACE_FunctionEntry() as the last line of the function
 *              These macros are valid also for ISRs.
 * @param[in]   funcAddr - Pass the function itself to the macro. This will
 *              trace the function's pointer into the trace buffer, which the
 *              analyzer converts back into a funciton name using the map file.
 *
 * Example usage:
 * void test_function(void)
 * {
 *     TRACE_FunctionEntry(test_function);
 *     // ... Do stuff ...
 *     TRACE_FunctionExit(test_function);
 * }
 *
 * Note:
 * It does not appear possible to get a pointer to the current function
 * automatically.  Hence why it is necessary to provide the pointers manually.
 * https://stackoverflow.com/questions/64261016/is-it-possible-to-unstringify-func-in-c
 */
#define TRACE_FunctionEntry(funcAddr)   TRACE_Put(                              \
    ((TRACE_IDCODE_FUNC_ENTRY << TRACE_IDCODE_Pos) & TRACE_IDCODE_Msk) |        \
    ((((uintptr_t)funcAddr - FLASH_BASE) << TRACE_DATA_Pos) & TRACE_DATA_Msk))
#define TRACE_FunctionExit(funcAddr)    TRACE_Put(                              \
    ((TRACE_IDCODE_FUNC_EXIT << TRACE_IDCODE_Pos) & TRACE_IDCODE_Msk) |         \
    ((((uintptr_t)funcAddr - FLASH_BASE) << TRACE_DATA_Pos) & TRACE_DATA_Msk))

/**
 * @brief       Trace file and line number
 * Note:        The file name itself is not traced. Instead the user must pass
 *              a "module identifier" that will be masked into the value.
 * param[in]    module - An integer identifier used to identify the file when
 *              analyzing the trace buffer.
 */
#define TRACE_Line(module)  TRACE_Put(                                          \
    ((TRACE_IDCODE_FILE_AND_LINE << TRACE_IDCODE_Pos) & TRACE_IDCODE_Msk) |     \
    ((TRACE_MODULE << TRACE_FANDL_MODULE_Pos) & TRACE_FANDL_MODULE_Msk) |       \
    ((__LINE__ << TRACE_FANDL_LINE_Pos) & TRACE_FANDL_LINE_Msk))

/**
 * @brief       Trace a variable value
 * Note:        This macro takes two entries in the buffer, one for the variables
 *              address and one for its value.
 * Note:        A variable must be global and non-static for it to appear in a
 *              gcc map file. Without this, the analyzer cannot name a variable
 *              explicitly when decoding the buffer. Instead, it will say
 *              Stack + N, Heap + N or similar or UNKNOWN.
 */
#define TRACE_VariableValue(var)    TRACE_Put(                                  \
    ((TRACE_IDCODE_VARIABLE_VALUE << TRACE_IDCODE_Pos) & TRACE_IDCODE_Msk) |    \
    ((((uintptr_t)(&var) - RAM_BASE) << TRACE_DATA_Pos) & TRACE_DATA_Msk));     \
    TRACE_Put((uint32_t)var)

/**
 * @brief       Trace a memory mapped peripheral register value
 * Note:        This macro takes two entries in the buffer, one for the variables
 *              address and one for its value.
 * Note:        Analysis of SFR traces requires the SVD file for your MCU.
 */
#define TRACE_SFRValue(reg)     TRACE_Put(                                      \
    ((TRACE_IDCODE_SFR_VALUE << TRACE_IDCODE_Pos) & TRACE_IDCODE_Msk) |         \
    ((((uintptr_t)(&reg) - RAM_BASE) << TRACE_DATA_Pos) & TRACE_DATA_Msk));     \
    TRACE_Put(reg)


typedef struct {
    uint32_t        magic;
    uint32_t        reset_count;
    uint32_t        head;
    uint32_t        tail;
    uint32_t        trace_buffer[BUFFER_LENGTH_IN_WORDS];
    uint32_t        last_value;
} ExecTracer_t;


extern volatile ExecTracer_t m_exec_trace;


/**
 * @brief       Initializes the trace buffer correctly for both power on and reset.
 * Note:        In the case noinit RAM is used, the buffer is preserved through
 *              reset.
 *              To capture traces after a crash scenario, all trace entries should
 *              be dumped to the back end after calling this function and before
 *              starting normal operation.
 */
void TRACE_Init(void);

#endif /* LIB_INCLUDE_EXECUTION_TRACER_H_ */
