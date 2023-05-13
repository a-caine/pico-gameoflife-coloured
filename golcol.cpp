#include "pico_display.hpp"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "button.hpp"
#include "pico/stdlib.h"
#include <stdint.h>

#define ROSC_RANDOMBIT_OFFSET _u(0x0000001c)
#define ROSC_BASE _u(0x40060000)

void updateCells();
void updateCell(uint16_t x, uint16_t y);
void setupCells();
void refreshDisplay();

volatile bool timer_fired = false;

bool timer_callback(struct repeating_timer *t) {
    refreshDisplay();

    return true;
}

const uint8_t CELL_SIZE = 2;
const uint8_t FPS = 5;

using namespace pimoroni;

ST7789 st7789(PicoDisplay::WIDTH, PicoDisplay::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);
Button reset_button(PicoDisplay::X);

struct Cell {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    bool c_alive;
    bool n_alive;
};

// Define a list of 'cells'
Cell cells[PicoDisplay::WIDTH / CELL_SIZE][PicoDisplay::HEIGHT / CELL_SIZE];

// return random number by ROSC between 0-2^N
// N should be in [0,32]
unsigned int ROrand(int N)
{
  static volatile uint32_t *randbit_reg = (uint32_t *)(ROSC_BASE + ROSC_RANDOMBIT_OFFSET);
  unsigned int random = 0;
  for (int i = 0; i < N; i++)
  {
    unsigned int random_bit = 0x1 & (*randbit_reg);
    random = random << 1 | random_bit;
  }
  return random;
}



int main() {

    stdio_init_all();

    // Initialise all of the cells
    setupCells();

    struct repeating_timer timer;
    add_repeating_timer_ms(-(1000 / FPS), timer_callback, NULL, &timer);

    

    // Give the chip some time to set up
    sleep_ms(500);

    while(true) {
        
        if (reset_button.raw()) {
            setupCells();
        }
        

    }
}

void refreshDisplay() {
    graphics.set_pen(0, 0, 0);
    graphics.clear();

    // Draw the grid
    for (uint16_t x = 0; x < PicoDisplay::WIDTH / CELL_SIZE; x++) {
        for (uint16_t y = 0; y < PicoDisplay::HEIGHT / CELL_SIZE; y++) {
            if (cells[x][y].c_alive) {
                    graphics.set_pen(cells[x][y].r, cells[x][y].g, cells[x][y].b);
                    graphics.rectangle(Rect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE));
            }
            
        }
    }

    st7789.update(&graphics);

    updateCells();
}

void setupCells() {
    for (uint16_t x = 0; x < PicoDisplay::WIDTH / CELL_SIZE; x++) {
        for (uint16_t y = 0; y < PicoDisplay::HEIGHT / CELL_SIZE; y++) {

            uint32_t val = ROrand(32);

            switch (val % 3) {
                case 0:
                    cells[x][y].r = 255;
                    cells[x][y].g = 0;
                    cells[x][y].b = 0;
                    break;
                case 1:
                    cells[x][y].r = 0;
                    cells[x][y].g = 255;
                    cells[x][y].b = 0;
                    break;
                case 2:
                    cells[x][y].r = 0;
                    cells[x][y].g = 0;
                    cells[x][y].b = 255;
                    break;
                default:
                    cells[x][y].r = 255;
                    cells[x][y].g = 255;
                    cells[x][y].b = 255;
            }

            val = ROrand(32);

            cells[x][y].c_alive = val % 2;
        }
    }
}

void updateCells() {
    // First of figure out the updates, then apply them
    for (uint16_t x = 0; x < PicoDisplay::WIDTH / CELL_SIZE; x++) {
        for (uint16_t y = 0; y < PicoDisplay::HEIGHT / CELL_SIZE; y++) {
            updateCell(x, y);
        }
    }

    // Then update the cells alive state
    for (uint16_t x = 0; x < PicoDisplay::WIDTH / CELL_SIZE; x++) {
        for (uint16_t y = 0; y < PicoDisplay::HEIGHT / CELL_SIZE; y++) {
            cells[x][y].c_alive = cells[x][y].n_alive;
            cells[x][y].n_alive = false;
        }
    }
}

void updateCell(uint16_t x, uint16_t y) {
    uint16_t aliveNeighbours = 0;
    uint16_t avg_r, avg_g, avg_b;

    for (int x_offset = -1; x_offset < 2; x_offset++) {
        for (int y_offset = -1; y_offset < 2; y_offset++) {
            if (x_offset == 0 && y_offset == 0) continue;
            if (x + x_offset < 0 || x + x_offset >= PicoDisplay::WIDTH / CELL_SIZE || y + y_offset < 0 || y + y_offset >= PicoDisplay::HEIGHT / CELL_SIZE) continue;

            Cell* cur_cell = &cells[x + x_offset][y + y_offset];

            if (!cur_cell->c_alive) continue;

            avg_r += cur_cell->r;
            avg_g += cur_cell->g;
            avg_b += cur_cell->b;
            aliveNeighbours++;
        }
    }

    Cell* cell = &cells[x][y];

    // Decide if the cell should be alive or not
    if (!cell->c_alive && aliveNeighbours == 3) {
        cell->n_alive = true;
        cell->r = (avg_r / 3) % 256;
        cell->g = (avg_g / 3) % 256;
        cell->b = (avg_b / 3) % 256;
    }

    if (cell->c_alive) {
        if (aliveNeighbours == 2 || aliveNeighbours == 3) {
            cell->n_alive = true;
        }
    }
}