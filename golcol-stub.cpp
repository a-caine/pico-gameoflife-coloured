#include "pico_display.hpp"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "button.hpp"
#include "pico/stdlib.h"
#include <stdint.h>

// Hardware offsets for the ring oscillator
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

// Dictates how many pixels (height and width) each cell should represent
const uint8_t CELL_SIZE = 2;
const uint8_t FPS = 5; // How many times a second the simulation should update

using namespace pimoroni;

ST7789 st7789(PicoDisplay::WIDTH, PicoDisplay::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);
Button reset_button(PicoDisplay::X);

/**
 * This structure represents a single cell in the simulation.
*/
struct Cell {
    uint8_t r; // the red value of the cell
    uint8_t g; // the green value of the cell
    uint8_t b; // the blue value of the cell
    bool c_alive; // whether this cell is currently alive
    bool n_alive; // whether this cell will be alive in the next iteration of the algorithm
};

// Define a list of 'cells'
Cell cells[PicoDisplay::WIDTH / CELL_SIZE][PicoDisplay::HEIGHT / CELL_SIZE];

// return random number by ROSC between 0-2^N
// N should be in the range [0,32] - this function does not support any higher
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

    struct repeating_timer timer; // Set up a timer to call the timer_callback every frame
    add_repeating_timer_ms(-(1000 / FPS), timer_callback, NULL, &timer);

    // Give the chip some time to set up
    sleep_ms(500);

    while(true) {
        
        // Infinitely check for the state of the reset button and set up the cells if pressed.
        if (reset_button.raw()) {
            setupCells();
        }
        

    }
}

/**
 * This function is called every frame and should render the current state of all cells onto the display.
*/
void refreshDisplay() {
    graphics.set_pen(0, 0, 0);
    graphics.clear();

    // Render the cells to the display here

    st7789.update(&graphics);

    updateCells();
}

/**
 * This function is called when the program starts, and also when the user presses the reset button.
*/
void setupCells() {
    // Initialise the cells with some random colour and alive state
    // Note: for random numbers use the function ROrand defined above, not the standard random functions
}

/**
 * This function is called once rendering the cells has finished.
*/
void updateCells() {
    // Update the state of cells according to the rules of Conway's game of life
}

/**
 * This function is not directly called, however may be useful to call from the updateCells function above.
*/
void updateCell(uint16_t x, uint16_t y) {
    // Update a single cell using the rules of Conway's game of life.
    // More information can be found here: https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
    // Also make sure to update the colours alongside the default rules.
}