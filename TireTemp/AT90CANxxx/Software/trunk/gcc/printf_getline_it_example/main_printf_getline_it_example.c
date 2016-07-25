//
//***** main_printf_getline_it_example.c ***************************************
//      Received and tansmitted bytes handled by interruption
//

#include "config.h"
#include "uart_it_lib.h"
#include "uart_it_drv.h"

extern U8 txt_logo_atmel[];

int main (void)
{
U8 uart0_str[]= ".....                ";
U8 uart1_str[]= ".....                ";
U8 u8_temp;

  // UART Selection
    Uart_select(UART_0);
  // UART Initialization
    uart_init(CONF_8BIT_NOPAR_1STOP,UART_BAUDRATE);
  
  // 'put_string' test  
	uart_put_string (txt_logo_atmel);
  // 'printf' test
    uart_mini_printf("... printf_getline_it_example ... UART_0 ...\r\n\n");
    u8_temp=0x05;
    uart_mini_printf("Hello world, the magic number is = 0x%X ... five !\r\n", u8_temp);
    u8_temp++;
    uart_mini_printf("Hello world, the magic number is = 0x%X ... six !\r\n", u8_temp);
    u8_temp++;
    uart_mini_printf("Hello world, the magic number is = 0x%X ... seven !\r\n", u8_temp);
    u8_temp++;
    uart_mini_printf("Hello world, the magic number is = 0x%X ... height !\r\n", u8_temp);
    u8_temp++;
    uart_mini_printf("Hello world, the magic number is = 0x%X ... nine !\r\n", u8_temp);
    uart_mini_printf("\r\n");
 
  // UART Selection
    Uart_select(UART_1);
  // UART Initialization
    uart_init(CONF_8BIT_NOPAR_1STOP,UART_BAUDRATE);
  
  // 'put_string' test  
	uart_put_string (txt_logo_atmel);
  // 'printf' test
    uart_mini_printf("... printf_getline_it_example ... UART_1 ...\r\n\n");
    u8_temp=0x15;
    uart_mini_printf("Hello world, the magic number is = 0x%X ... fifteen !\r\n", u8_temp);
    u8_temp++;
    uart_mini_printf("Hello world, the magic number is = 0x%X ... fifty-sixteen !\r\n", u8_temp);
    u8_temp++;
    uart_mini_printf("Hello world, the magic number is = 0x%X ... fifty-seventeen !\r\n", u8_temp);
    u8_temp++;
    uart_mini_printf("Hello world, the magic number is = 0x%X ... eighteen !\r\n", u8_temp);
    u8_temp++;
    uart_mini_printf("Hello world, the magic number is = 0x%X ... nineteen !\r\n", u8_temp);
    uart_mini_printf("\r\n");
 
    while(1) // No-Stop !
    {
        // get character test
        Uart_select(UART_0);
		u8_temp = uart_test_hit();
        if (u8_temp)
		{
	        // "uart_get_editedline()" function goes out only when its buffer is full
			//                         or when a CR is entered 
			uart_get_editedline(&uart0_str[5], 13);
            uart_mini_printf("\r\nGot: %s\r\n", &uart0_str[5]);
            uart_put_string (uart0_str);
            uart_mini_printf("\r\n");
        }
        // get character test
        Uart_select(UART_1);
		u8_temp = uart_test_hit();
        if (u8_temp)
		{
	        // "uart_get_editedline()" function goes out only when its buffer is full
			//                         or when a CR is entered 
	        uart_get_editedline(&uart1_str[5], 13);
            uart_mini_printf("\r\nGot: %s\r\n", &uart1_str[5]);
            uart_put_string (uart1_str);
            uart_mini_printf("\r\n");
        }
    }
    return 0;
}

// Example:
// --------
// Hello world, the magic number is = 0x55 ... fifty-five !
// Hello world, the magic  ...
// ...
// AaBbCcDdEe
// Got: AaBbCcDdEe
// .....AaBbCcDdEe
//
