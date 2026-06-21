#include "send.h"
#include <stdint.h>

static void write_cmd(uint8_t cmd) {
    gpio_put(DC_PIN, 0);
    gpio_put(CS_PIN, 0);
    spi_write_blocking(SPI_DEVICE, &cmd, 1);
    gpio_put(CS_PIN, 1);
}

static void write_data(const uint8_t *data, size_t len) {
    gpio_put(DC_PIN, 1);
    gpio_put(CS_PIN, 0);
    spi_write_blocking(SPI_DEVICE, data, len);
    gpio_put(CS_PIN, 1);
}

void init_display() {
    spi_init(SPI_DEVICE, 1000 * 1000);
    spi_set_format(SPI_DEVICE, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_set_function(MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SCK_PIN, GPIO_FUNC_SPI);

    gpio_set_function(CS_PIN, GPIO_FUNC_SIO);
    gpio_set_dir(CS_PIN, GPIO_OUT);
    gpio_put(CS_PIN, 1);

    gpio_init(DC_PIN);
    gpio_set_dir(DC_PIN, GPIO_OUT);
    gpio_put(DC_PIN, 1);

    gpio_init(RST_PIN);
    gpio_set_dir(RST_PIN, GPIO_OUT);
    gpio_put(RST_PIN, 1);

    // Hardware reset
    gpio_put(RST_PIN, 0);
    sleep_ms(20);
    gpio_put(RST_PIN, 1);
    sleep_ms(120);

    // ST7789 minimal init
    write_cmd(0x01); // SWRESET
    sleep_ms(150);
    write_cmd(0x11); // SLPOUT
    sleep_ms(120);

    // COLMOD: 16-bit/pixel (RGB565)
    write_cmd(0x3A);
    uint8_t colmod = 0x55;
    write_data(&colmod, 1);

    // MADCTL: default orientation
    write_cmd(0x36);
    uint8_t madctl = 0x00;
    write_data(&madctl, 1);

    // Normal display on + inversion on (per user report)
    write_cmd(0x13); // NORON
    write_cmd(0x21); // INVON

    write_cmd(0x29); // DISPON
    sleep_ms(20);
}

void display_set_inversion(bool invert) {
    write_cmd(invert ? 0x21 : 0x20);
}

void display_buffer() {
    const int w = FRAME_WIDTH;
    const int h = FRAME_HEIGHT;
    static uint8_t line_buf[FRAME_WIDTH * 2];

    // Find depth range for visible pixels to improve contrast
    uint8_t min_d = 255;
    uint8_t max_d = 0;
    bool any = false;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            uint8_t d = frame_buffer->depth[y][x];
            if (d < 255) {
                if (d < min_d) min_d = d;
                if (d > max_d) max_d = d;
                any = true;
            }
        }
    }
    if (!any || min_d == max_d) {
        min_d = 0;
        max_d = 255;
    }

    uint8_t caset[] = { 0x00, 0x00, (uint8_t)((w - 1) >> 8), (uint8_t)((w - 1) & 0xFF) };
    uint8_t raset[] = { 0x00, 0x00, (uint8_t)((h - 1) >> 8), (uint8_t)((h - 1) & 0xFF) };

    write_cmd(0x2A); // CASET
    write_data(caset, sizeof(caset));
    write_cmd(0x2B); // RASET
    write_data(raset, sizeof(raset));

    // RAMWR then stream pixel data
    gpio_put(DC_PIN, 0);
    gpio_put(CS_PIN, 0);
    uint8_t ramwr = 0x2C;
    spi_write_blocking(SPI_DEVICE, &ramwr, 1);
    gpio_put(DC_PIN, 1);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            uint8_t r = frame_buffer->color[y][x][0];
            uint8_t g = frame_buffer->color[y][x][1];
            uint8_t b = frame_buffer->color[y][x][2];
            uint8_t d = frame_buffer->depth[y][x];

            // Depth-based darkening normalized to visible depth range
            uint16_t shade = (uint16_t)((max_d - d) * 255u / (max_d - min_d));
            r = (uint8_t)((r * shade) / 255);
            g = (uint8_t)((g * shade) / 255);
            b = (uint8_t)((b * shade) / 255);

            uint16_t rgb565 = (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
            line_buf[x * 2 + 0] = (uint8_t)(rgb565 >> 8);
            line_buf[x * 2 + 1] = (uint8_t)(rgb565 & 0xFF);
        }

        spi_write_blocking(SPI_DEVICE, line_buf, w * 2);
    }

    gpio_put(CS_PIN, 1);
}