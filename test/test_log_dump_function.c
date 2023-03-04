/*
 * test_log_dump_function.c
 *
 *  Created on: Mar 4, 2023
 *      Author: AFont
 */

/* Include files ----------------------------------------------------------- */
#include <stdio.h>
#include <string.h>

#include "unity.h"
#include "execution_tracer.h"
#include "helper_functions.h"

/* Private macros ---------------------------------------------------------- */
#define TRACE_MODULE        1
#define ARRAY_SIZE(a)       (sizeof(a) / sizeof(a[0]))

/* Private variables ------------------------------------------------------- */
static bool       m_write_called;
static bool       m_lock_called;
static bool       m_unlock_called;
static int        m_num_writes_expected;
static int        m_num_writes_actual;
static uint32_t * m_expected_write_values;

/* Set up and tear down ---------------------------------------------------- */
void setUp(void)
{
    m_write_called = false;
    m_lock_called = false;
    m_unlock_called = false;
    m_num_writes_expected = 0;
    m_num_writes_actual = 0;
    TRACE_Clear();
}

void tearDown(void)
{
}

/* Helper functions -------------------------------------------------------- */
void write(uint8_t * p_data, uint16_t size)
{
    m_write_called = true;
}
void write_with_check(uint8_t * p_data, uint16_t size)
{
    char test_buff[16];
    m_write_called = true;
    snprintf(test_buff, 16, "0x%08X\n", m_expected_write_values[m_num_writes_actual++]);
    TEST_ASSERT_EQUAL(strlen(test_buff), size);
    TEST_ASSERT_EQUAL(0, memcmp(test_buff, p_data, size));
}
void lock(void)
{
    m_lock_called = true;
}
void unlock(void)
{
    m_unlock_called = true;
}
ExecTraceCallbacks_t test_callbacks_with_lock = {
        .write = write,
        .lock = lock,
        .unlock = unlock
};
ExecTraceCallbacks_t test_callbacks_without_lock = {
        .write = write,
        .lock = NULL,
        .unlock = NULL
};
ExecTraceCallbacks_t test_callbacks_with_write_check = {
        .write = write_with_check,
        .lock = NULL,
        .unlock = NULL
};

/* Test functions ---------------------------------------------------------- */
void test_CallbackFunctionsAreInvoked(void)
{
    TRACE_Init(&test_callbacks_with_lock);
    DumpExecTraceLog();
    TEST_ASSERT_TRUE(m_lock_called);
    TEST_ASSERT_TRUE(m_write_called);
    TEST_ASSERT_TRUE(m_unlock_called);
}

void test_DumpLogWorksWithoutLockFunctions(void)
{
    TRACE_Init(&test_callbacks_without_lock);
    DumpExecTraceLog();
    TEST_ASSERT_FALSE(m_lock_called);
    TEST_ASSERT_TRUE(m_write_called);
    TEST_ASSERT_FALSE(m_unlock_called);
}

void test_Uint32sAreCorrectlyDecodedToHexStrings(void)
{
    uint32_t test_values[] = {
            0x12345678,
            0x00000000,
            0xFFFFFFFF,
            0xFEDCBA98,
            0x80000000,
            0x7FFFFFFF,
    };

    TRACE_Init(&test_callbacks_with_write_check);
    TRACE_Clear();

    for(int i = 0; i < ARRAY_SIZE(test_values); i++)
    {
        TRACE_Put(test_values[i]);
    }

    m_num_writes_expected = ARRAY_SIZE(test_values);
    m_expected_write_values = test_values;
    DumpExecTraceLog();
    TEST_ASSERT_EQUAL(m_num_writes_expected, m_num_writes_actual);
}

void test_WriteFunctionNotCalledWhenTraceBufferIsEmpty(void)
{
    TRACE_Init(&test_callbacks_with_lock);
    TRACE_Clear();
    DumpExecTraceLog();
    TEST_ASSERT_TRUE(m_lock_called);
    TEST_ASSERT_FALSE(m_write_called);
    TEST_ASSERT_TRUE(m_unlock_called);
}
