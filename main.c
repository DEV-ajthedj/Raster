#include "draw.h"
#include "send.h"

void init() {
    stdio_init_all();

    // Frame Buffer Initialization
    init_buffer();

    // World Data Initialization
    init_world();

    // Display initialization
    init_display();
    display_set_inversion(false);

    sleep_ms(2000);
    printf("Initialization complete.\n");
}

int main() {
    init();

    init_world();
    fill_buffer(255, (unsigned char[COLOR_CHANNELS]){0, 0, 0});
    update_buffer((vector3_t){0, 0, 0});
    print_frame();

    double s = 0.0;

    while (1) {
        display_buffer();
        sleep_ms(1000 / 10); // 10 FPS
        s += 0.1;
    }

    return 0;
}