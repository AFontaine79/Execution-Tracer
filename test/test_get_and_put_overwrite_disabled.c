/*
 * test_get_and_put_overwrite_disabled.c
 *
 *  Created on: Oct 23, 2022
 *      Author: Aaron Fontaine
 */


#include "unity.h"
#include "execution_tracer.h"

void setUp(void)
{
    TRACE_Clear();
}

void tearDown(void)
{

}

void test_NewTraceBufferIsEmptyAndNotFull(void)
{
    TEST_ASSERT_EQUAL(true, TRACE_IsEmpty());
    TEST_ASSERT_EQUAL(false, TRACE_IsFull());
}
