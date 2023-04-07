/*
 * execution_tracer_conf.h
 *
 *  Created on: Oct 22, 2022
 *      Author: Aaron Fontaine
 */

#ifndef LIB_INCLUDE_EXECUTION_TRACER_CONF_H_
#define LIB_INCLUDE_EXECUTION_TRACER_CONF_H_

/**
 * To save buffer space, function entry and exit traces are performed as
 * offsets from the start of flash.  FLASH_BASE should equal the start of
 * FLASH space on your system.
 */
/* Standard Arm Cortex FLASH base */
// #define FLASH_BASE		(0x00000000)
/* Standard STM32 FLASH base */
#define FLASH_BASE (0x08000000)

/**
 * Variable and SFR addresses are also stored as offsets.
 */
/* Standard Arm Cortex RAM base */
#define RAM_BASE (0x20000000)
/* Standard STM32 peripheral registers region base address */
#define SFR_BASE (0x40000000)

/**
 * Execution tracer settings.
 */

#define STOP_TRACING_AFTER_RESET        1

#define USE_NOINIT_RAM_FOR_TRACING      1

#define ALLOW_OVERWRITE                 1

#define BUFFER_LENGTH_IN_WORDS          1024

#endif /* LIB_INCLUDE_EXECUTION_TRACER_CONF_H_ */
