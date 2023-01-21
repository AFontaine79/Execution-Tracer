/*
 * execution_tracer_conf.h
 *
 *  Created on: Oct 22, 2022
 *      Author: Aaron Fontaine
 */

#ifndef LIB_INCLUDE_EXECUTION_TRACER_CONF_H_
#define LIB_INCLUDE_EXECUTION_TRACER_CONF_H_

/* All CONFIG_ settings come from Ceedling project.yml file */

#define FLASH_BASE                      CONFIG_FLASH_BASE
#define RAM_BASE                        CONFIG_RAM_BASE
#define SFR_BASE                        CONFIG_SFR_BASE

#define STOP_TRACING_AFTER_RESET        CONFIG_STOP_AFTER_RESET
#define USE_NOINIT_RAM_FOR_TRACING      CONFIG_USE_NO_INIT
#define ALLOW_OVERWRITE                 CONFIG_ALLOW_OVERWRITE
#define BUFFER_LENGTH_IN_WORDS          CONFIG_BUFFER_LENGTH

#endif /* LIB_INCLUDE_EXECUTION_TRACER_CONF_H_ */
