/*
 * Arm FastModels Hello World Boot Example
 *
 * Basic String Library
 * File: string.c
 *
 * Copyright 2020 Reto Achermann
 * SPDX-License-Identifier: GPL-3.0
 */

#include <string.h>

void bzero(void *buf, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        ((char *)buf)[i] = 0;
    }
}