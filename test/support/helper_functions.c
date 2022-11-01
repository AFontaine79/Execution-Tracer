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
