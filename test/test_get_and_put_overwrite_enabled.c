/*
 * test_get_and_put_overwrite_enabled.c
 *
 *  Created on: Nov 12, 2022
 *      Author: AFont
 */


#include "unity.h"
#include "execution_tracer.h"
#include "helper_functions.h"

#define TEST_VALUE_1        0x11111111
#define TEST_VALUE_2        0x22222222
#define TEST_VALUE_3        0x33333333

#define TEST_VALUE_A        0xAAAAAAAA
#define TEST_VALUE_C        0xCCCCCCCC

void setUp(void)
{
    TRACE_Clear();
}

void tearDown(void)
{
}

void test_PutOverwritesOldestValueWhenQueueIsFull(void)
{
    uint32_t    lastHeadValue;
    uint32_t    lastTailValue;

    /* Ensure head and tail pointer start halfway through queue */
    /* Being the first entry, value C will get overwritten */
    helper_WriteNEntriesToQueue(TEST_VALUE_1, BUFFER_LENGTH_IN_WORDS / 2);
    helper_EmptyQueue();
    TRACE_Put(TEST_VALUE_C);
    helper_WriteNEntriesToQueue(TEST_VALUE_1, BUFFER_MAX_CAPACITY - 1);
    lastHeadValue = m_exec_trace.head;
    lastTailValue = m_exec_trace.tail;

    /* Put of value A succeeds */
    TRACE_Put(TEST_VALUE_A);
    TEST_ASSERT_EQUAL_UINT32(++lastHeadValue, m_exec_trace.head);
    TEST_ASSERT_EQUAL_UINT32(++lastTailValue, m_exec_trace.tail);

    /* Oldest item in queue should be value 1; value C is gone */
    TEST_ASSERT_EQUAL_UINT32(TEST_VALUE_1, TRACE_Get());

    /* First item has already been tested; All remaining except last are value 1 */
    helper_VerifyNEntriesInQueue(TEST_VALUE_1, BUFFER_MAX_CAPACITY - 2);

    /* Last item in queue is value A that overwrote value C */
    TEST_ASSERT_EQUAL_UINT32(TEST_VALUE_A, TRACE_Get());
    TEST_ASSERT_TRUE(TRACE_IsEmpty());
}

void test_PutOverwritesOldestValueWhenQueueIsFullAtWraparound(void)
{
    /* Tail starts at 0; Head will end up at end of queue */
    /* Being the first entry, value C will get overwritten */
    TRACE_Put(TEST_VALUE_C);
    helper_WriteNEntriesToQueue(TEST_VALUE_1, BUFFER_MAX_CAPACITY - 1);
    TEST_ASSERT_EQUAL_UINT32(BUFFER_MAX_CAPACITY, m_exec_trace.head);
    TEST_ASSERT_EQUAL_UINT32(0, m_exec_trace.tail);

    /* Put of value A succeeds */
    TRACE_Put(TEST_VALUE_A);
    TEST_ASSERT_EQUAL_UINT32(0, m_exec_trace.head);
    TEST_ASSERT_EQUAL_UINT32(1, m_exec_trace.tail);

    /* Oldest item in queue should be value 1 */
    TEST_ASSERT_EQUAL_UINT32(TEST_VALUE_1, TRACE_Get());

    /* First item has already been tested; All remaining except last are value 1 */
    helper_VerifyNEntriesInQueue(TEST_VALUE_1, BUFFER_MAX_CAPACITY - 2);

    /* Last item in queue is value A that overwrote value C */
    TEST_ASSERT_EQUAL_UINT32(TEST_VALUE_A, TRACE_Get());
    TEST_ASSERT_TRUE(TRACE_IsEmpty());
}
