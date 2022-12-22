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
#define TRACE_IDCODE_INVALID        0
#define TRACE_IDCODE_VERSION        1
#define TRACE_IDCODE_RESET          2

/* Check whether BUFFER_LENGTH_IN_WORDS is a power of 2 is in .c file */
#define BUFFER_INDEX_MASK       (BUFFER_LENGTH_IN_WORDS - 1)
#define BUFFER_MAX_CAPACITY     (BUFFER_LENGTH_IN_WORDS - 1)

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
#define TRACE_ExecTracerVersion()   TRACE_Put(                  \
    ((TRACE_IDCODE_VERSION & 0xF) << 28) |                      \
    (('V' & 0xFF) << 16) |                                      \
    ((TRACE_PROTOCOL_MAJOR & 0xFF) << 8) |                      \
    (TRACE_PROTOCOL_MINOR & 0xFF))

/**
 * @brief       Traces information about the processor reset
 *              Call this once during system startup.
 * @param[in]   reset_reg - User defined, the lower 28 bits are available.
 *              This should be the contents of the processor's reset register
 *              (E.g. RCC->CSR).  Shift and mask the register value to include
 *              only the relevant bits and avoid writing bits 31 through 28.
 */
#define TRACE_ProcessorReset(reset_reg) TRACE_Put(              \
    ((TRACE_IDCODE_RESET & 0xF) << 28) |                        \
    (reset_reg & 0xFFFFFFF))

#define TRACE_FunctionEntry()
#define TRACE_FunctionExit()
#define TRACE_Line()
#define TRACE_VariableValue()
#define TRACE_SFRValue()


typedef struct {
    uint32_t        magic;
    uint32_t        reset_count;
    uint32_t        head;
    uint32_t        tail;
    uint32_t        trace_buffer[BUFFER_LENGTH_IN_WORDS];
    uint32_t        last_value;
} ExecTracer_t;


extern ExecTracer_t m_exec_trace;

#endif /* LIB_INCLUDE_EXECUTION_TRACER_H_ */
