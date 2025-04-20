/**
 * Copyright (c) 2025 Collin Kidder
 *
 * MIT License
 */

#include <stdlib.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "pio_isospi.h"

//connect your TX pins to 6,7,8,9 or change this value but it always is consecutive from this number
#define TX_BASE 6
//connect your RX pins to 10,11
#define RX_BASE 10


int main() 
{
    stdio_init_all();

    pio_isospi_inst_t spi = {
            .pio = pio0,
            .sm = 0
    };
    uint tx_prog_offs = pio_add_program(spi.pio, &isospi_tx_program);
    //uint rx_prog_offs = pio_add_program(spi.pio, &isospi_rx_program);

    sleep_ms(2000);
    printf("Trying to init PIO stuff\n");
    pio_isospi_init(spi.pio, spi.sm, tx_prog_offs, TX_BASE, RX_BASE );
    while (1)
    {
        sleep_ms(1000);
        printf("About to try to place a 32 bit value in TX FIFO\n");
        pio_sm_put(spi.pio, spi.sm, 0xAA21FF57);
    }
}
    

