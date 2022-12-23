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

#define TEST_VALUE_A            0xAAAAAAAA
#define TEST_VALUE_B            0xBBBBBBBB

void tracedFunction(void)
{
    TRACE_FunctionEntry(tracedFunction);
    TRACE_FunctionExit(tracedFunction);
}

#define FIRST_TRACED_LINE       33
#define SECOND_TRACED_LINE      34
void tracedLines(void)
{
    _Static_assert(__LINE__ == 32, "Adjust FIRST_TRACED_LINE, SECOND_TRACED_LINE and this check");
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

uint32_t testVariable = TEST_VALUE_A;
void test_TraceVariable(void)
{
    TRACE_VariableValue(testVariable);
    helper_VerifyVariableTrace(&testVariable, testVariable);
}

/* SFRs don't exist when testing on the host, so there's not much that can be
 * done here other than faking it out. TRACE_SFRValue works the same as
 * TRACE_VariableValue, except with different ID code and memory offset.
 */
uint32_t someMemoryMappedPeripheralRegister = TEST_VALUE_B;
void test_TraceSFR(void)
{
    TRACE_SFRValue(someMemoryMappedPeripheralRegister);
    helper_VerifySFRTrace(&someMemoryMappedPeripheralRegister,
            someMemoryMappedPeripheralRegister);
}
