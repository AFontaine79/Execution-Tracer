/*
 * test_get_and_put_overwrite_disabled.c
 *
 *  Created on: Oct 23, 2022
 *      Author: Aaron Fontaine
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

void test_NewQueueIsEmptyAndNotFull(void)
{
    TEST_ASSERT_TRUE(TRACE_IsEmpty());
    TEST_ASSERT_FALSE(TRACE_IsFull());
}

void test_GetReturnsPreviouslyWrittenValue(void)
{
    TRACE_Put(TEST_VALUE_1);
    TEST_ASSERT_EQUAL_UINT32(TEST_VALUE_1, TRACE_Get());

    TRACE_Put(TEST_VALUE_2);
    TEST_ASSERT_EQUAL_UINT32(TEST_VALUE_2, TRACE_Get());

    TRACE_Put(TEST_VALUE_1);
    TRACE_Put(TEST_VALUE_2);
    TRACE_Put(TEST_VALUE_3);
    TEST_ASSERT_EQUAL_UINT32(TEST_VALUE_1, TRACE_Get());
    TEST_ASSERT_EQUAL_UINT32(TEST_VALUE_2, TRACE_Get());
    TEST_ASSERT_EQUAL_UINT32(TEST_VALUE_3, TRACE_Get());
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
    TEST_ASSERT_EQUAL_UINT32(TEST_VALUE_1, TRACE_Get());
    TEST_ASSERT_FALSE(TRACE_IsEmpty());
    TEST_ASSERT_EQUAL_UINT32(TEST_VALUE_2, TRACE_Get());
    TEST_ASSERT_FALSE(TRACE_IsEmpty());
    TEST_ASSERT_EQUAL_UINT32(TEST_VALUE_3, TRACE_Get());
    TEST_ASSERT_TRUE(TRACE_IsEmpty());
}

void test_QueueReportsFullWhenHeadAndTailWrapAround(void)
{
    helper_FillEntireQueueWithValue(TEST_VALUE_C);
    TEST_ASSERT_TRUE(TRACE_IsFull());
}

void test_QueueReportsFullWhenHeadAndTailAreAdjacent(void)
{
    /* Offset so we don't test wraparound condition */
    TRACE_Put(TEST_VALUE_1);
    TRACE_Get();

    helper_FillEntireQueueWithValue(TEST_VALUE_C);
    TEST_ASSERT_TRUE(TRACE_IsFull());

    /* Move forward five spaces and repeat test */
    for(int index = 0; index < 5; index++) {
        TRACE_Get();
        TRACE_Put(TEST_VALUE_C);
    }
    TEST_ASSERT_TRUE(TRACE_IsFull());

    /* Final test with head and tail at last and 2nd-to-last elements */
    for(int index = 0; index < (BUFFER_MAX_CAPACITY - 6); index++) {
        TRACE_Get();
        TRACE_Put(TEST_VALUE_C);
    }
    TEST_ASSERT_TRUE(TRACE_IsFull());
}

void test_GetThenPutWithQueueFullAtWraparound(void)
{
    test_QueueReportsFullWhenHeadAndTailAreAdjacent();
    TRACE_Get();
    TRACE_Put(TEST_VALUE_C);
    TEST_ASSERT_TRUE(TRACE_IsFull());
}

void test_NewQueueHasZeroEntries(void)
{
    TEST_ASSERT_EQUAL_UINT32(0, TRACE_GetNumEntries());
}

void test_GetNumEntriesWorksForNonWrapAround(void)
{
    helper_WriteNEntriesToQueue(TEST_VALUE_C, 5);
    TEST_ASSERT_EQUAL_UINT32(5, TRACE_GetNumEntries());
    helper_WriteNEntriesToQueue(TEST_VALUE_C, 5);
    TEST_ASSERT_EQUAL_UINT32(10, TRACE_GetNumEntries());
    helper_RemoveNEntriesFromQueue(3);
    TEST_ASSERT_EQUAL_UINT32(7, TRACE_GetNumEntries());
    helper_RemoveNEntriesFromQueue(6);
    TEST_ASSERT_EQUAL_UINT32(1, TRACE_GetNumEntries());
}

void test_GetNumEntriesWorksForWrapAround(void)
{
    helper_WriteNEntriesToQueue(TEST_VALUE_C, BUFFER_MAX_CAPACITY / 2);
    helper_EmptyQueue();

    helper_FillEntireQueueWithValue(TEST_VALUE_C);
    TEST_ASSERT_EQUAL_UINT32(BUFFER_MAX_CAPACITY, TRACE_GetNumEntries());

    TRACE_Get();
    TEST_ASSERT_EQUAL_UINT32(BUFFER_MAX_CAPACITY - 1, TRACE_GetNumEntries());
    TRACE_Get();
    TEST_ASSERT_EQUAL_UINT32(BUFFER_MAX_CAPACITY - 2, TRACE_GetNumEntries());
    TRACE_Get();
    TEST_ASSERT_EQUAL_UINT32(BUFFER_MAX_CAPACITY - 3, TRACE_GetNumEntries());
}
