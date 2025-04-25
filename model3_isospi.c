/**
 * Copyright (c) 2025 Collin Kidder
 *
 * MIT License
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/structs/bus_ctrl.h"
#include "pio_isospi.h"

//connect your TX pins to 6,7,8,9 or change this value but it always is consecutive from this number
#define TX_BASE 6
//connect your RX pins to 10,11
#define RX_BASE 10

/*polling version of TX code. Try to place as many bytes into FIFO as we can. The FIFO
is 32 bits but we're only using 8 so that we can send as individual bytes if needed. 
Many messages are only 2 bytes so we do need this functionality. FIFO should be 4 long
so we can queue 4 bytes then have to busy wait until we can push in another. 
*/
void __time_critical_func(isospi_write8_blocking)(const uint8_t *src, size_t len) {
    size_t tx_remain = len;

    while (tx_remain) {
        if (tx_remain && !pio_sm_is_tx_fifo_full(pio0, 0)) {
            pio0->txf[0] = (*src++) << 24ul; //we read only from the top byte
            --tx_remain;
        }
    }
}

/*Here, though, we set up to use the whole 32 bits of each of the 4 FIFO buffers. Capturing
2 bits at a time (the state of the high/low pulse capture pins). DMA driven so we don't
have to worry about polling or the fact that this will all be running at the same time
as the above polling code.
*/
void read_isospi_arm(PIO pio, uint sm, uint dma_chan, uint32_t *capture_buf, size_t capture_size_words,
    uint trigger_pin, bool trigger_level)
{
    pio_sm_set_enabled(pio, sm, false);
    // Need to clear _input shift counter_, as well as FIFO, because there may be
    // partial ISR contents left over from a previous run. sm_restart does this.
    pio_sm_clear_fifos(pio, sm);
    pio_sm_restart(pio, sm);

    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_read_increment(&c, false); //always read from FIFO which is at a static address
    channel_config_set_write_increment(&c, true); //but we are going to have to auto increment the pointer into the buffer
    channel_config_set_dreq(&c, pio_get_dreq(pio, sm, false));

    dma_channel_configure(dma_chan, &c,
        capture_buf,        // Destination pointer
        &pio->rxf[sm],      // Source pointer
        capture_size_words, // Number of transfers
        true                // Start immediately
    );

    //don't actually start up until the RX pins start to transition from the TX going live
    pio_sm_exec(pio, sm, pio_encode_wait_gpio(trigger_level, trigger_pin));
    pio_sm_set_enabled(pio, sm, true);
}

void print_capture_buf(const uint32_t *buf, uint pin_base, uint pin_count, uint32_t n_samples) {
    // Display the capture buffer in text form, like this:
    // 00: __--__--__--__--__--__--
    // 01: ____----____----____----
    printf("Capture:\n");
    uint record_size_bits = 32;
    for (uint pin = 0; pin < pin_count; ++pin) {
        printf("%02d: ", pin + pin_base);
        for (uint32_t sample = 0; sample < n_samples; ++sample) {
            uint bit_index = pin + sample * pin_count;
            uint word_index = bit_index / record_size_bits;
            // Data is left-justified in each FIFO entry, hence the (32 - record_size_bits) offset
            uint word_mask = 1u << (bit_index % record_size_bits + 32 - record_size_bits);
            printf(buf[word_index] & word_mask ? "-" : "_");
        }
        printf("\n");
    }
}

int main() 
{
    stdio_init_all();
    uint8_t message[90];
    uint dma_chan = 0;

    pio_isospi_inst_t spi = {
            .pio = pio0,
            .sm = 0
    };
    uint tx_prog_offs = pio_add_program(spi.pio, &isospi_tx_program);
    uint rx_prog_offs = pio_add_program(spi.pio, &isospi_rx_program);

    uint32_t *capture_buf = malloc(500 * sizeof(uint32_t));
    hard_assert(capture_buf);
    memset(capture_buf, 0, 500 * sizeof(uint32_t));

    // Grant high bus priority to the DMA, so it can shove the processors out
    // of the way. This should only be needed if you are pushing things up to
    // >16bits/clk here, i.e. if you need to saturate the bus completely.
    //We are not actually using that much memory transfer so this is probably not necessary
    bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;


    sleep_ms(2000);
    printf("Trying to init PIO stuff\n");
    pio_isospi_init(spi.pio, spi.sm, tx_prog_offs, rx_prog_offs, TX_BASE, RX_BASE );
    while (1)
    {
        sleep_ms(1000);
        printf("About to try a 2 byte message\n");
        message[0] = 0x21;
        message[1] = 0xF2;
        read_isospi_arm(pio0, 1, dma_chan, capture_buf, 500, RX_BASE, true);
        isospi_write8_blocking(message, 2);
        dma_channel_wait_for_finish_blocking(dma_chan);
        print_capture_buf(capture_buf, RX_BASE, 2, 2500);
        sleep_ms(1000);
        printf("About to try a long message\n");
        message[0] = 0x47;
        message[1] = 0xA1;
        message[2] = 0xCF;
        for (int i = 3; i < 76; i++) message[i] = 0;
        isospi_write8_blocking(message, 76);
        //sleep_ms(1000);
        //printf("About to try pushing a 32 bit value with put\n");
        //pio_sm_put(spi.pio, spi.sm, 0xAA21FF57);        
    }
}

/*
To capture a whole 76 byte message we need to handle 76us worth of traffic plus the CS up and down which
collectively aren't more than an additional 1us. So, 77us worth of traffic. 
We're capturing at 80Mhz which is 12.5ns so we will be taking around 6160 samples, 16 per 32 bit FIFO
transfer so 385 transfers and 1540 bytes 
*/
    

