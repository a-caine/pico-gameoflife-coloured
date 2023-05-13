# Conways Game of Life - Coloured

The aim of this project is to simulate [Conway's game of life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life), but with an extra parameter; each cell in this variant of Conway's game of life has a colour associated with it.
When a cell is born from parent cells, it should inherit each of the parents colours and mix them appropriately.
For instance, a cell with a fully red parent (#FF0000), a fully green parent (#00FF00) and a fully blue parent (#0000FF) should be born with equal amounts of red green and blue (#555555).

## Set-Up

This project relies upon the pimoroni pico display library, instructions on how to set this up can be found [here](https://github.com/pimoroni/pimoroni-pico#cc).

This project also relies upon the standard pico-sdk, instructions on how to set this up can be found [here](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)

This repository contains both a stubbed example, and a full solution should you get stuck at any point.