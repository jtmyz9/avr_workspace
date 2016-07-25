//
//***** main_printf_getline_example.c ***************************************************

#include "config.h"
#include "uart_lib.h"
#include "uart_drv.h"


int main (void)
{
U8 uart_str[]= ".....                ";
U8 u8_temp;

  // UART Initialization
    uart_init(CONF_8BIT_NOPAR_1STOP,UART_BAUDRATE);
    
  // 'printf' test
    uart_mini_printf("\r\n\n... printf_getline_example ...\r\n");
    u8_temp=0x55;
    uart_mini_printf("Hello world, the magic number is = 0x%X ... fifty-five !\r\n", u8_temp);
    u8_temp++;
    uart_mini_printf("Hello world, the magic number is = 0x%X ... fifty-six !\r\n", u8_temp);
    u8_temp++;
    uart_mini_printf("Hello world, the magic number is = 0x%X ... fifty-seven !\r\n", u8_temp);
    u8_temp++;
    uart_mini_printf("Hello world, the magic number is = 0x%X ... fifty-height !\r\n", u8_temp);
    u8_temp++;
    uart_mini_printf("Hello world, the magic number is = 0x%X ... fifty-nine !\r\n", u8_temp);
    u8_temp++;
 
    while(1) // No-Stop !
    {
        // get characters test
        uart_get_editedline(&uart_str[5], 13);
        uart_mini_printf("\r\nGot: %s\r\n", &uart_str[5]);
        uart_put_string (uart_str);
        uart_mini_printf("\r\n");
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
