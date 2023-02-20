/*
 * exeuction_tracer.c
 *
 *  Created on: Oct 23, 2022
 *      Author: Aaron Fontaine
 */

#include "execution_tracer.h"
#include "execution_tracer_private.h"

_Static_assert(IS_POWER_OF_2(BUFFER_LENGTH_IN_WORDS), "BUFFER_LENGTH_IN_WORDS must be a power of 2");

__NO_INIT ExecTracer_t m_exec_trace;

void TRACE_Init(void)
{
    if (m_exec_trace.magic != EXEC_TRACE_INIT_MAGIC)
    {
        m_exec_trace.reset_count = 0;
        m_exec_trace.head = 0;
        m_exec_trace.tail = 0;
        m_exec_trace.magic = EXEC_TRACE_INIT_MAGIC;
    }
    else
    {
        m_exec_trace.reset_count++;
    }
    TRACE_ExecTracerVersion();
}
