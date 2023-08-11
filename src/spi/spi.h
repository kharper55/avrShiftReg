#ifndef SPI_H_
#define SPI_H_

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

void spi_master_init();
// made static... void spi_send(uint8_t* tx, uint8_t* rx);
void spi_transfer(uint8_t* tx, uint8_t* rx, uint16_t len);

#endif // SPI_H_