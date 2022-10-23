/*
 * execution_tracer.h
 *
 *  Created on: Oct 22, 2022
 *      Author: Aaron Fontaine
 */

#ifndef LIB_INCLUDE_EXECUTION_TRACER_H_
#define LIB_INCLUDE_EXECUTION_TRACER_H_

#include "execution_tracer_conf.h"

/* Check whether BUFFER_LENGTH_IN_WORDS is a power of 2 is in .c file */
#define BUFFER_INDEX_MASK   (BUFFER_LENGTH_IN_WORDS - 1)

#define TRACE_IsEmpty()     (m_exec_trace.head == m_exec_trace.tail)
#define TRACE_IsFull()      (((m_exec_trace.head + 1) & BUFFER_INDEX_MASK) == m_exec_trace.tail)

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
    do {                                                        \
        if (!TRACE_IsFull()) {                                  \
            m_exec_trace.trace_buffer[m_exec_trace.head++] = n; \
            m_exec_trace.head &= BUFFER_INDEX_MASK;             \
        }                                                       \
    } while (0)
#endif

#define TRACE_ExecTracerVersion()   TRACE_Put(                  \
    ((TRACE_IDCODE_VERSION & 0xF) << 28) &                      \
    (('V' & 0xFF) << 16) &                                      \
    ((TRACE_PROTOCOL_MAJOR & 0xFF) << 16) &                     \
    ((TRACE_PROTOCOL_MINOR & 0xFF) << 16))

#define TRACE_ProcessorReset()
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
} ExecTracer_t;


extern ExecTracer_t m_exec_trace;

#endif /* LIB_INCLUDE_EXECUTION_TRACER_H_ */
