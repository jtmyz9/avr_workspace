/*
 * old.c
 *
 *  Created on: Jan 10, 2012
 *      Author: Administrator
 */


	printf("-Start-\n");
	printf("Waiting\n");
	_delay_ms(5000);

	if(glcd_init() != 0)
	{
		printf("GLCD Init Failed\n");
		while(1);
	}
	printf("GLCD Initialized\n");
	/*
	printf("Changing Baudrate\n");
	resp = glcd_set_baud(GLCD_14400_BAUD,14400);
	if(resp != 0)
	{
		printf("Change Baud Failed - resp: 0x%02X\n",resp);
		while(1);
	}
	printf("Baudrate changed\n");
	*/


	printf("Clear Screen\n");
	glcd_clrscr();
	printf("Screen Cleared\n");

	glcd_setfont_bkgrnd(GLCD_FONT_OPAQ);

	glcd_setpen(GLCD_NOFILL);

	glcd_puts_gph(15,10,GLCD_FONT_12X16,GLCD_WHITE,1,1,"Water Temp (F)");

	glcd_rectangle(10,30,148,70,GLCD_WHITE);
	glcd_rectangle(10,60,148,70,GLCD_WHITE);
	glcd_puts_gph(17,62,GLCD_FONT_5X7,LIGHTCYAN,1,1,"100");
	glcd_line(35,60,35,70,GLCD_WHITE);
	glcd_puts_gph(45,62,GLCD_FONT_5X7,GREENYELLOW,1,1,"150");
	glcd_line(63,60,63,70,GLCD_WHITE);
	glcd_puts_gph(74,62,GLCD_FONT_5X7,YELLOW,1,1,"200");
	glcd_line(92,60,92,70,GLCD_WHITE);
	glcd_puts_gph(103,62,GLCD_FONT_5X7,ORANGE,1,1,"250");
	glcd_line(121,60,121,70,GLCD_WHITE);
	glcd_puts_gph(130,62,GLCD_FONT_5X7,RED,1,1,"300");

	while(1)
	{
		temp += 10;
		if(temp > 300)	temp = 50;

		glcd_setpen(GLCD_FILL);
		glcd_rectangle(11,31,147,59,DARKGRAY);
		glcd_setpen(GLCD_NOFILL);

		if(temp > 250)		color = RED;
		else if(temp > 200)	color = ORANGE;
		else if(temp > 150) color = YELLOW;
		else if(temp > 100)	color = GREENYELLOW;
		else				color = LIGHTCYAN;

		if(temp <= 50)	bar_size = 0;
		else			bar_size = ((temp - 50)*136)/250;

		if(bar_size != 0)
		{
			glcd_setpen(GLCD_FILL);
			glcd_rectangle(11,33,10+bar_size,57,color);
			glcd_setpen(GLCD_NOFILL);
		}
		sprintf(str,"   ");
		glcd_puts_gph(163,42,GLCD_FONT_8X8,color,3,3,str);

		sprintf(str,"%u",temp);

		glcd_puts_gph(163,42,GLCD_FONT_8X8,color,3,3,str);

		_delay_ms(1000);

	}


	/*
	glcd_puts_gph(15,90,GLCD_FONT_12X16,GLCD_WHITE,1,1,"Air Temp (F)");
	glcd_rectangle(10,110,230,150,GLCD_WHITE);
	glcd_puts_gph(15,170,GLCD_FONT_12X16,GLCD_WHITE,1,1,"Oil Temp (F)");
	glcd_rectangle(10,190,230,230,GLCD_WHITE);
	glcd_puts_gph(15,250,GLCD_FONT_12X16,GLCD_WHITE,1,1,"Oil Pres (Psi)");
	glcd_rectangle(10,270,230,310,GLCD_WHITE);
*/
