/*
 * test_get_and_put_overwrite_disabled.c
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

void test_PutDoesNothingWhenQueueIsFull(void)
{
    uint32_t    lastHeadValue;
    uint32_t    lastTailValue;

    /* Ensure head and tail pointer start halfway through queue */
    helper_WriteNEntriesToQueue(TEST_VALUE_1, BUFFER_LENGTH_IN_WORDS / 2);
    helper_EmptyQueue();
    helper_FillEntireQueueWithValue(TEST_VALUE_1);
    lastHeadValue = m_exec_trace.head;
    lastTailValue = m_exec_trace.tail;

    /* Put of value A fails */
    TRACE_Put(TEST_VALUE_A);
    TEST_ASSERT_EQUAL_UINT32(lastHeadValue, m_exec_trace.head);
    TEST_ASSERT_EQUAL_UINT32(lastTailValue, m_exec_trace.tail);

    /* Oldest item in queue should be value 1 */
    TEST_ASSERT_EQUAL_UINT32(TEST_VALUE_1, TRACE_Get());

    /* Fill new slot with value 1; Head and tail both increment */
    TRACE_Put(TEST_VALUE_1);
    TEST_ASSERT_EQUAL_UINT32(++lastHeadValue, m_exec_trace.head);
    TEST_ASSERT_EQUAL_UINT32(++lastTailValue, m_exec_trace.tail);

    /* Put of value A fails */
    TRACE_Put(TEST_VALUE_A);
    TEST_ASSERT_EQUAL_UINT32(lastHeadValue, m_exec_trace.head);
    TEST_ASSERT_EQUAL_UINT32(lastTailValue, m_exec_trace.tail);

    /* Verify that only put 1 succeeded and put As failed */
    helper_VerifyEntireQueue(TEST_VALUE_1);
}

void test_PutDoesNothingWhenQueueIsFullAtWraparound(void)
{
    /* Tail starts at 0; Head will end up at end of queue */
    helper_FillEntireQueueWithValue(TEST_VALUE_1);
    TEST_ASSERT_EQUAL_UINT32(BUFFER_MAX_CAPACITY, m_exec_trace.head);
    TEST_ASSERT_EQUAL_UINT32(0, m_exec_trace.tail);

    /* Put of value A fails */
    TRACE_Put(TEST_VALUE_A);
    TEST_ASSERT_EQUAL_UINT32(BUFFER_MAX_CAPACITY, m_exec_trace.head);
    TEST_ASSERT_EQUAL_UINT32(0, m_exec_trace.tail);

    /* Oldest item in queue should be value 1 */
    TEST_ASSERT_EQUAL_UINT32(TEST_VALUE_1, TRACE_Get());

    /* Fill new slot with value 1; Head and tail both increment */
    TRACE_Put(TEST_VALUE_1);
    TEST_ASSERT_EQUAL_UINT32(0, m_exec_trace.head);
    TEST_ASSERT_EQUAL_UINT32(1, m_exec_trace.tail);

    /* Put of value A fails */
    TRACE_Put(TEST_VALUE_A);
    TEST_ASSERT_EQUAL_UINT32(0, m_exec_trace.head);
    TEST_ASSERT_EQUAL_UINT32(1, m_exec_trace.tail);

    /* Verify that only put 1 succeeded and put As failed */
    helper_VerifyEntireQueue(TEST_VALUE_1);
}
