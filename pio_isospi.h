/**
 * Copyright (c) 2025 Collin Kidder
 *
 * MIT License
 */
#ifndef _PIO_ISOSPI_H
#define _PIO_ISOSPI_H

#include "hardware/pio.h"
#include "isospi.pio.h"

typedef struct pio_isospi_inst {
    PIO pio;
    uint sm;
} pio_isospi_inst_t;

//void pio_spi_write8_blocking(const pio_spi_inst_t *spi, const uint8_t *src, size_t len);

//void pio_spi_read8_blocking(const pio_spi_inst_t *spi, uint8_t *dst, size_t len);

//void pio_spi_write8_read8_blocking(const pio_spi_inst_t *spi, uint8_t *src, uint8_t *dst, size_t len);

#endif
