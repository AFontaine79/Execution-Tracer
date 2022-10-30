/*
 * test_get_and_put_overwrite_disabled.c
 *
 *  Created on: Oct 23, 2022
 *      Author: Aaron Fontaine
 */


#include "unity.h"
#include "execution_tracer.h"

#define TEST_VALUE_1        0x11111111
#define TEST_VALUE_2        0x22222222
#define TEST_VALUE_3        0x33333333

void setUp(void)
{
    TRACE_Clear();
}

void tearDown(void)
{

}

void helper_FillEntireQueueWithValue(uint32_t value)
{
    TEST_ASSERT_EQUAL(true, TRACE_IsEmpty());
    for(int count = 0; count < (BUFFER_MAX_CAPACITY - 1); count++)
    {
        TRACE_Put(value);
        TEST_ASSERT_EQUAL(false, TRACE_IsFull());
    }
    TRACE_Put(value);
    TEST_ASSERT_EQUAL(true, TRACE_IsFull());
}

void test_NewTraceQueueReportsIsEmptyAsTrueAndNotFullAsFalse(void)
{
    TEST_ASSERT_EQUAL(true, TRACE_IsEmpty());
    TEST_ASSERT_EQUAL(false, TRACE_IsFull());
}

void test_GetReturnsPreviouslyWrittenValue(void)
{
    TRACE_Put(TEST_VALUE_1);
    TEST_ASSERT_EQUAL(TEST_VALUE_1, TRACE_Get());

    TRACE_Put(TEST_VALUE_2);
    TEST_ASSERT_EQUAL(TEST_VALUE_2, TRACE_Get());

    TRACE_Put(TEST_VALUE_1);
    TRACE_Put(TEST_VALUE_2);
    TRACE_Put(TEST_VALUE_3);
    TEST_ASSERT_EQUAL(TEST_VALUE_1, TRACE_Get());
    TEST_ASSERT_EQUAL(TEST_VALUE_2, TRACE_Get());
    TEST_ASSERT_EQUAL(TEST_VALUE_3, TRACE_Get());
}

void test_GetReturnsZeroIfQueueIsEmpty(void)
{
    TEST_ASSERT_EQUAL(0, TRACE_Get());
    test_GetReturnsPreviouslyWrittenValue();
    TEST_ASSERT_EQUAL(0, TRACE_Get());
}

void test_QueueReportsEmptyAfterAllElementsRemoved(void)
{

    TRACE_Put(TEST_VALUE_1);
    TRACE_Put(TEST_VALUE_2);
    TRACE_Put(TEST_VALUE_3);
    TEST_ASSERT_EQUAL(TEST_VALUE_1, TRACE_Get());
    TEST_ASSERT_EQUAL(false, TRACE_IsEmpty());
    TEST_ASSERT_EQUAL(TEST_VALUE_2, TRACE_Get());
    TEST_ASSERT_EQUAL(false, TRACE_IsEmpty());
    TEST_ASSERT_EQUAL(TEST_VALUE_3, TRACE_Get());
    TEST_ASSERT_EQUAL(true, TRACE_IsEmpty());
}

void test_QueueReportsFullWhenFilledCompletely(void)
{
    helper_FillEntireQueueWithValue(0xCCCCCCCC);
}