/*
 * exeuction_tracer.c
 *
 *  Created on: Oct 23, 2022
 *      Author: Aaron Fontaine
 */

#include "execution_tracer.h"
#include "execution_tracer_private.h"

#include <stdint.h>
#include <stdbool.h>

_Static_assert(IS_POWER_OF_2(BUFFER_LENGTH_IN_WORDS), "BUFFER_LENGTH_IN_WORDS must be a power of 2");

__NO_INIT ExecTracer_t m_exec_trace;
