/*
 * test_trace_functions.c
 *
 *  Created on: Dec 22, 2022
 *      Author: AFont
 */

#include <stdio.h>

#include "unity.h"
#include "execution_tracer.h"
#include "helper_functions.h"

#define FAKE_RCC_CSR_VALUE      0x20000000                      /* IWDG reset on ST MCU */
#define RESET_TEST_VALUE        (FAKE_RCC_CSR_VALUE >> 24)      /* Reset flags use the top 8 bits of RCC->CSR */

void setUp(void)
{
    TRACE_Clear();
}

void tearDown(void)
{
}

void tracedFunction(void)
{
    TRACE_FunctionEntry(tracedFunction);
    TRACE_FunctionExit(tracedFunction);
}


void test_TraceExecTracerVersion(void)
{
    TRACE_ExecTracerVersion();
    helper_VerifyExecTracerVersionTrace();
}

void test_TraceProcessorReset(void)
{
    TRACE_ProcessorReset(RESET_TEST_VALUE);
    helper_VerifyProcessorResetTrace(RESET_TEST_VALUE);
}

void test_TraceFunctionEntryAndExit(void)
{
    uint32_t entryValue;
    uint32_t exitValue;

    tracedFunction();
    entryValue = TRACE_Get();
    TEST_ASSERT_EQUAL_UINT8(TRACE_IDCODE_FUNC_ENTRY, (uint8_t)((entryValue >> 28) & 0xF));
    exitValue = TRACE_Get();
    TEST_ASSERT_EQUAL_UINT8(TRACE_IDCODE_FUNC_EXIT, (uint8_t)((exitValue >> 28) & 0xF));
    entryValue &= 0xFFFFFFF;
    exitValue &= 0xFFFFFFF;
    TEST_ASSERT_EQUAL(entryValue, exitValue);
    printf("Verify in the map file that tracedFunction() is at 0x%08X", (entryValue + FLASH_BASE));
}
