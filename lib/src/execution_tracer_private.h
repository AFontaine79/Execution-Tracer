/*
 * execution_tracer_private.h
 *
 *  Created on: Oct 23, 2022
 *      Author: Aaron Fontaine
 */

#ifndef LIB_SRC_EXECUTION_TRACER_PRIVATE_H_
#define LIB_SRC_EXECUTION_TRACER_PRIVATE_H_

#define NOINIT_VALID_SIGNATURE		0xAA55CC33

/* TODO: Determine whether MDK-ARM (Keil) noinit works the same as GCC */
#if defined(__GNUC__) || defined(__CC_ARM)
#define __NO_INIT   __attribute__((section(".noinit")))
#elif defined(__ICCARM__)
#define __NO_INIT   __no_init
#endif

#define IS_POWER_OF_2(n)        ((n & (n - 1)) == 0)

#endif /* LIB_SRC_EXECUTION_TRACER_PRIVATE_H_ */
