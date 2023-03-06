/*
 * exeuction_tracer.c
 *
 *  Created on: Oct 23, 2022
 *      Author: Aaron Fontaine
 */

#include <string.h>
#include "execution_tracer.h"
#include "execution_tracer_private.h"

_Static_assert(IS_POWER_OF_2(BUFFER_LENGTH_IN_WORDS), "BUFFER_LENGTH_IN_WORDS must be a power of 2");

/* Private variables ------------------------------------------------------- */
/**
 * The globally accessible execution tracer object.
 */
__NO_INIT ExecTracer_t volatile m_exec_trace;

/**
 * Callbacks for implementation defined by the client.
 */
static ExecTraceCallbacks_t m_exec_trace_callbacks = {0};

/**
 * Array to simplify conversion of uints to ASCII (e.g. %x formatting) without
 * relying on heavyweight printf function.
 */
static const char m_hex_to_ascii[16] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

/* Private function prototypes --------------------------------------------- */
void _ConvertUint32ToHexString(uint32_t value, char * out_buffer);

/* Public functions -------------------------------------------------------- */
void TRACE_Init(ExecTraceCallbacks_t * p_callbacks)
{
    m_exec_trace_callbacks = *p_callbacks;

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

void DumpExecTraceLog(void)
{
    static char out_buffer[] = "0x00000000\n";
    uint32_t trace_value;

    if (m_exec_trace_callbacks.lock)
    {
        m_exec_trace_callbacks.lock();
    }

    if (TRACE_IsFull())
    {
        memset(&out_buffer[2], m_hex_to_ascii[TRACE_IDCODE_BUFFER_FULL], 8);
        m_exec_trace_callbacks.write((uint8_t*)out_buffer, 11);
    }
    while (!TRACE_IsEmpty())
    {
        trace_value = TRACE_Get();
        _ConvertUint32ToHexString(trace_value, &out_buffer[2]);
        m_exec_trace_callbacks.write((uint8_t*)out_buffer, 11);
    }

    if (m_exec_trace_callbacks.unlock)
    {
        m_exec_trace_callbacks.unlock();
    }
}

/* Private functions ------------------------------------------------------- */
void _ConvertUint32ToHexString(uint32_t value, char * out_buffer)
{
    char * p_out = out_buffer;
    int shift_amount = 28;
    for(int i = 0; i < 8; i++)
    {
        *p_out = m_hex_to_ascii[(value >> shift_amount) & 0xF];
        shift_amount -= 4;
        p_out++;
    }
}
