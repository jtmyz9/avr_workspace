//
//***** main_printf_gcc_example.c ***************************************************

#include "config.h"
#include "uart_gcc_drv.h"

FILE uart_str = FDEV_SETUP_STREAM(uart_put_char, uart_get_char, _FDEV_SETUP_RW);

int main (void)
{
char buf[20], s[20];
U8 u8_temp=0x55;

  // UART Initialization
    uart_init();
  // 'printf' initialization
    stdout = stdin = stderr = &uart_str;
    
  // 'printf' test
    printf ("Hello world, the magic value is = 0x%X ... fifty-five !\r\n", u8_temp);
    u8_temp++;
 
	// get characters test
	uart_get_char(&uart_str);
	fprintf(&uart_str, "Got: %s\n", s);
	
	while(1); // Stop !
    return 0;
}
