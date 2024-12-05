/*
 * Arm FastModels Hello World Boot
 *
 * Copyright 2020 Reto Achermann
 * SPDX-License-Identifier: GPL-3.0
 */

#ifndef LOGGING_H_
#define LOGGING_H_ 1

#include <stdio.h>

///< a basic message printing macro
#define MSG(format, ...) printf("\033[97;1m[ARMv8]: " format "\033[0m", ## __VA_ARGS__ )

#define WARN(format, ...) printf("\033[33;1m[ARMv8]: WARNING " format "\033[0m", ## __VA_ARGS__ )

#define ERROR(format, ...) printf("\033[91;1m[ARMv8]: ERROR " format "\033[0m", ## __VA_ARGS__ )

#endif /* LOGGING_H_ */