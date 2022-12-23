/*
 * helper_functions.c
 *
 *  Created on: Oct 30, 2022
 *      Author: Aaron Fontaine
 */

#include "unity.h"
#include "execution_tracer.h"
#include "helper_functions.h"


void helper_FillEntireQueueWithValue(uint32_t value)
{
    /* Queue not empty when calling this function is a test setup failure */
    TEST_ASSERT_TRUE(TRACE_IsEmpty());
    helper_WriteNEntriesToQueue(value, BUFFER_MAX_CAPACITY);
}

void helper_WriteNEntriesToQueue(uint32_t value, uint32_t n)
{
    for(int index = 0; index < n; index++)
    {
        TRACE_Put(value);
    }
}

void helper_VerifyEntireQueue(uint32_t value)
{
    while(!TRACE_IsEmpty())
    {
        TEST_ASSERT_EQUAL_UINT32(value, TRACE_Get());
    }
}

void helper_VerifyNEntriesInQueue(uint32_t value, uint32_t n)
{
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(n, TRACE_GetNumEntries());
    for(int index = 0; index < n; index++)
    {
        TEST_ASSERT_EQUAL_UINT32(value, TRACE_Get());
    }

}

void helper_RemoveNEntriesFromQueue(uint32_t n)
{
    for(int index = 0; index < n; index++)
    {
        TRACE_Get();
    }
}

void helper_EmptyQueue(void)
{
    while(!TRACE_IsEmpty())
    {
        TRACE_Get();
    }
}

void helper_VerifyExecTracerVersionTrace(void)
{
    uint32_t value = TRACE_Get();
    TEST_ASSERT_EQUAL_UINT8(TRACE_IDCODE_VERSION, (uint8_t)((value & TRACE_IDCODE_Msk) >> TRACE_IDCODE_Pos));
    TEST_ASSERT_EQUAL_UINT8(TRACE_VERSION_V_Val, (uint8_t)((value & TRACE_VERSION_V_Msk) >> TRACE_VERSION_V_Pos));
    TEST_ASSERT_EQUAL_UINT8(TRACE_PROTOCOL_MAJOR, (uint8_t)((value & TRACE_VERSION_MAJOR_Msk) >> TRACE_VERSION_MAJOR_Pos));
    TEST_ASSERT_EQUAL_UINT8(TRACE_PROTOCOL_MINOR, (uint8_t)((value & TRACE_VERSION_MINOR_Msk) >> TRACE_VERSION_MINOR_Pos));
}

void helper_VerifyProcessorResetTrace(uint32_t exp_value)
{
    uint32_t value = TRACE_Get();
    TEST_ASSERT_EQUAL_UINT8(TRACE_IDCODE_RESET, (uint8_t)((value & TRACE_IDCODE_Msk) >> TRACE_IDCODE_Pos));
    TEST_ASSERT_EQUAL_UINT8(exp_value, (uint8_t)((value & TRACE_DATA_Msk) >> TRACE_DATA_Pos));
}

void helper_VerifyLineTrace(uint32_t module, uint32_t line_num)
{
    uint32_t value = TRACE_Get();
    TEST_ASSERT_EQUAL_UINT8(TRACE_IDCODE_FILE_AND_LINE, (uint8_t)((value & TRACE_IDCODE_Msk) >> TRACE_IDCODE_Pos));
    TEST_ASSERT_EQUAL_UINT8(module, (uint8_t)((value & TRACE_FANDL_MODULE_Msk) >> TRACE_FANDL_MODULE_Pos));
    TEST_ASSERT_EQUAL_UINT8(line_num, (uint8_t)((value & TRACE_FANDL_FILE_Msk) >> TRACE_FANDL_FILE_Pos));
}
