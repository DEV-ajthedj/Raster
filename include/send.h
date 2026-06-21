#ifndef SEND_H
#define SEND_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/spi.h"
#include "draw.h"

// SPI pins (adjust if your wiring differs)
#define SPI_DEVICE spi0
#define MOSI_PIN 19
#define SCK_PIN 18
#define CS_PIN 21
#define DC_PIN 17
#define RST_PIN 22

void init_display();
void display_buffer();
void display_set_inversion(bool invert);

#endif