/*
 * Copyright (c) 2023 iAchieved.it LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <zephyr.h>
#include <syscalls/rand32.h>
#include <stdio.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/spi.h>

#define SPI_FREQUENCY 20000000

const device* spi;
struct spi_config cfg;

void writeSRAM(uint16_t address, const uint8_t data) {
  uint8_t h[] = {0x02, (address & 0xff00) >> 8, (address & 0xff), data};

  const struct spi_buf tx = {
    .buf = h,
    .len = 4
  };

  const struct spi_buf_set tx_bufs = {
    .buffers = (const struct spi_buf*)&tx,
    .count   = 1
  };

  spi_write(spi, &cfg, &tx_bufs);
}

void writeSRAM(uint16_t address, const uint8_t* data, uint8_t len) {
  
  uint8_t h[] = {0x02, (address & 0xff00) >> 8, (address & 0xff)};

  const struct spi_buf tx[] = {{
    .buf = h,
    .len = 3
  },{
	  .buf = (void*)data,
	  .len = len
  }};

  const struct spi_buf_set tx_bufs = {
	  .buffers = (const struct spi_buf*)&tx,
    .count = 2
  };

  spi_write(spi, &cfg, &tx_bufs);

}

void readSRAM(uint16_t address, uint8_t* data, uint8_t len) {

  uint8_t addr_h = (address & 0xff00) >> 8;
  uint8_t addr_l = address & 0xff;
  uint8_t header[] = {0x03, addr_h, addr_l};

  const struct spi_buf tx[] = {{
      .buf = header,
      .len = 3
  },{
    .buf = (void*)data,
    .len = len
  }};
    
  const struct spi_buf rx[]= {
    {
      .buf = NULL,
      .len = 3
    },
    {
      .buf = (void*)data,
      .len = len
    }
  };

  const struct spi_buf_set tx_buf_set = {
    .buffers = (const struct spi_buf*)&tx,
    .count   = 2
  };

  const struct spi_buf_set rx_buf_set = {
    .buffers = (const struct spi_buf*)&rx,
    .count   = 2
  };

  spi_transceive(spi, &cfg, &tx_buf_set, &rx_buf_set);

}

#define BUFLEN 8

int main(void) {

  printf("Hello, world!\n");

  spi = DEVICE_DT_GET(DT_NODELABEL(spi1));
  cfg.frequency = SPI_FREQUENCY;
	cfg.operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8);

  uint8_t tx[BUFLEN];
  uint8_t rx[BUFLEN];

  while (true) {

    for (int i = 0; i < BUFLEN; i++) {
      tx[i] = (uint8_t)sys_rand32_get();
    }

    writeSRAM(0xcc, tx, BUFLEN);
    k_msleep(1000);
    readSRAM(0xcc, rx, BUFLEN);
    k_msleep(1000);

    for (int i = 0; i < BUFLEN; i++ ) {

      printf("%x ", rx[i]);
    } printf("\n");

  }

  return 0;


}