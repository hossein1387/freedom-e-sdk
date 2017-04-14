// See LICENSE for license details.

// This is the program which ships on the HiFive1
// board, executing out of SPI Flash at 0x20400000.

#include <stdint.h>
#include <stdio.h>
#include "platform.h"

static const char init_msg[] = "Testing GPIO maximum speed";

static void _putc(char c) {
  while ((int32_t) UART0_REG(UART_REG_TXFIFO) < 0);
  UART0_REG(UART_REG_TXFIFO) = c;
}

int _getc(char * c){
  int32_t val = (int32_t) UART0_REG(UART_REG_RXFIFO);
  if (val > 0) {
    *c =  val & 0xFF;
    return 1;
  }
  return 0;
}


static void _puts(const char * s) {
  while (*s != '\0'){
    _putc(*s++);
  }
}

static void init_uart(){
    // Configure UART to print
  GPIO_REG(GPIO_OUTPUT_VAL) |= IOF0_UART0_MASK;
  GPIO_REG(GPIO_OUTPUT_EN)  |= IOF0_UART0_MASK;
  GPIO_REG(GPIO_IOF_SEL)    &= ~IOF0_UART0_MASK;
  GPIO_REG(GPIO_IOF_EN)     |= IOF0_UART0_MASK;

  // 115200 Baud Rate
  UART0_REG(UART_REG_DIV) = 138;
  UART0_REG(UART_REG_TXCTRL) = UART_TXEN;
  UART0_REG(UART_REG_RXCTRL) = UART_RXEN;

  // Wait a bit to avoid corruption on the UART.
  // (In some cases, switching to the IOF can lead
  // to output glitches, so need to let the UART
  // reciever time out and resynchronize to the real 
  // start of the stream.
  volatile int i=0;
  while(i < 10000){i++;}
}

int main (void){

  // Run off 16 MHz Crystal for accuracy.
  PRCI_REG(PRCI_PLLCFG) = (PLL_REFSEL(1) | PLL_BYPASS(1));
  PRCI_REG(PRCI_PLLCFG) |= (PLL_SEL(1));
  // Turn off HFROSC to save power
  PRCI_REG(PRCI_HFROSCCFG) = 0;
  
  init_uart();
  _puts(init_msg);
  

  GPIO_REG(GPIO_IOF_SEL)    |= (1 << PIN_2_OFFSET);
  GPIO_REG(GPIO_OUTPUT_EN)  |= (1 << PIN_2_OFFSET);  
  GPIO_REG(GPIO_IOF_EN )    |= (1 << PIN_2_OFFSET);
  GPIO_REG(GPIO_OUTPUT_XOR) &= ~(1 << PIN_2_OFFSET);
//  GPIO_REG(GPIO_OUTPUT_XOR) |= (1 << RED_LED_OFFSET);
  GPIO_REG(GPIO_OUTPUT_VAL) |= (1 << PIN_2_OFFSET);
  while(1){
      GPIO_REG(GPIO_OUTPUT_XOR) &= ~(1 << PIN_2_OFFSET);
      GPIO_REG(GPIO_OUTPUT_XOR) |= (1 << PIN_2_OFFSET);

  }

 return 0;
}
