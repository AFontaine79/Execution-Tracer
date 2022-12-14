/*
 * helper_functions.h
 *
 *  Created on: Oct 30, 2022
 *      Author: Aaron Fontaine
 */

#ifndef TEST_SUPPORT_HELPER_FUNCTIONS_H_
#define TEST_SUPPORT_HELPER_FUNCTIONS_H_

#include <stdbool.h>
#include <stdint.h>

void helper_FillEntireQueueWithValue(uint32_t value);
void helper_WriteNEntriesToQueue(uint32_t value, uint32_t n);
void helper_VerifyEntireQueue(uint32_t value);
void helper_VerifyNEntriesInQueue(uint32_t value, uint32_t n);
void helper_RemoveNEntriesFromQueue(uint32_t n);
void helper_EmptyQueue(void);

void helper_VerifyExecTracerVersionTrace(void);
void helper_VerifyProcessorResetTrace(uint32_t exp_value);
void helper_VerifyLineTrace(uint32_t module, uint32_t line_num);
void helper_VerifyVariableTrace(uint32_t * exp_addr, uint32_t exp_value);
void helper_VerifySFRTrace(uint32_t * exp_addr, uint32_t exp_value);

#endif /* TEST_SUPPORT_HELPER_FUNCTIONS_H_ */
