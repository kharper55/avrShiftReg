#include "spi.h"

volatile uint8_t spi_rx_data = 0;
volatile uint8_t spi_txrx_done = 0;

ISR(SPI_STC_vect) {
    if (SPSR & (1 << WCOL)) { // Write collision occurred
        spi_rx_data = SPDR;
        spi_rx_data = 0;
        spi_txrx_done = 1;
    }
    else {
        spi_rx_data = SPDR;
        spi_txrx_done = 1;
    }
}

//PB4 - MISO
//PB3 - MOSI
//PB5 - SCK
//PB2 - SS~

// Init SPI as master
void spi_master_init() {
    DDRB &= ~(1 << DDB4);                              // Set MISO to input
    DDRB |= (1 << DDB2) | (1 << DDB3) | (1 << DDB5);   // Set MOSI, SCK, & SS to output
    PORTB |= (1 << PB2);                               // Set SS high (active low functionality)
    // Enable interrupt on SPSR bit SPIF, enable SPI peripheral, set clock polarity to idle high,
    // sample on trailing edge of clock phase (with setup on leading edge), set SPI prescaler to 128
    SPCR = (1 << SPIE) | (1 << SPE) | (1 << MSTR) | (1 << CPOL) | (0 << CPHA) | (0b11 << SPR0);    
    // DORD bit in SPCR specifes data order; with 0 (rst val.) MSB is transmitted first              
}

static spi_send(uint8_t* tx, uint8_t* rx) {
    spi_txrx_done = 0;
    SPDR = *tx;

    while (spi_txrx_done == 0) {
        *rx = spi_rx_data;
    }

}

void spi_transfer(uint8_t* tx, uint8_t* rx, uint16_t len) {

    PORTB &= ~(1 << PB2);
    _delay_us(100);

    for (uint16_t i = 0; i < len; i++) {
        spi_send(&tx[i], &rx[i]);
    }

    PORTB |= (1 << PB2);
    _delay_us(100);

}