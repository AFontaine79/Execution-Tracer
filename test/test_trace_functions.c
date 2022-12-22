/*
 * test_trace_functions.c
 *
 *  Created on: Dec 22, 2022
 *      Author: AFont
 */


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
