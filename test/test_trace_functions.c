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

#define TRACE_MODULE        1

#define FAKE_RCC_CSR_VALUE      0x20000000                      /* IWDG reset on ST MCU */
#define RESET_TEST_VALUE        (FAKE_RCC_CSR_VALUE >> 24)      /* Reset flags use the top 8 bits of RCC->CSR */

void tracedFunction(void)
{
    TRACE_FunctionEntry(tracedFunction);
    TRACE_FunctionExit(tracedFunction);
}

#define FIRST_TRACED_LINE       30
#define SECOND_TRACED_LINE      31
void tracedLines(void)
{
    _Static_assert(__LINE__ == 29, "Adjust FIRST_TRACED_LINE, SECOND_TRACED_LINE and this check");
    TRACE_Line(TRACE_MODULE);
    TRACE_Line(TRACE_MODULE);
}

void setUp(void)
{
    TRACE_Clear();
}

void tearDown(void)
{
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

void test_TraceLine(void)
{
    tracedLines();
    helper_VerifyLineTrace(TRACE_MODULE, FIRST_TRACED_LINE);
    helper_VerifyLineTrace(TRACE_MODULE, SECOND_TRACED_LINE);
}
