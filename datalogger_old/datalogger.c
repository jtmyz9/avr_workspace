/*
 * datalogger.c
 *
 *  Created on: Dec 30, 2011
 *      Author: Mark
 */

#include "datalogger.h"

uint8_t print_disk_info(const struct fat_fs_struct* fs);

struct partition_struct* partition;
struct fat_fs_struct* fsys;
struct fat_dir_entry_struct dir_ent;
struct fat_dir_struct* dir;

char fat_init(void)
{
	if(!sd_raw_init()) return -1;
	partition =	partition_open(sd_raw_read,sd_raw_read_interval,sd_raw_write,sd_raw_write_interval,0);

	if(!partition)
	{
		partition = partition_open(sd_raw_read,sd_raw_read_interval,sd_raw_write,sd_raw_write_interval,-1);
		if(!partition)	return(-2);
	}

	fsys = fat_open(partition);

	if(!fsys)	return(-3);

	dir = fat_open_dir(fsys, &dir_ent);

	if(!dir) return(-4);

	return(0);

}

int main()
{
	struct fat_file_struct *f;

	uart_init();

	uart_puts_p(PSTR("------| START |------\n\r"));

	if(!fat_init())
	{
		uart_puts_p(PSTR("FAT ErRoR\r\n\n"));
	}

	uart_puts_p(PSTR("Fat Initialized!\r\n\n"));



	while(1);
}

uint8_t print_disk_info(const struct fat_fs_struct* fs)
{
    if(!fs)
        return 0;

    struct sd_raw_info disk_info;
    if(!sd_raw_get_info(&disk_info))
        return 0;

    uart_puts_p(PSTR("manuf:  0x")); uart_putc_hex(disk_info.manufacturer); uart_putc('\n\r');uart_putc('\r');
    uart_puts_p(PSTR("oem:    ")); uart_puts((char*) disk_info.oem); uart_putc('\n');uart_putc('\r');
    uart_puts_p(PSTR("prod:   ")); uart_puts((char*) disk_info.product); uart_putc('\n');uart_putc('\r');
    uart_puts_p(PSTR("rev:    ")); uart_putc_hex(disk_info.revision); uart_putc('\n');uart_putc('\r');
    uart_puts_p(PSTR("serial: 0x")); uart_putdw_hex(disk_info.serial); uart_putc('\n');uart_putc('\r');
    uart_puts_p(PSTR("date:   ")); uart_putw_dec(disk_info.manufacturing_month); uart_putc('/');
                                   uart_putw_dec(disk_info.manufacturing_year); uart_putc('\n');uart_putc('\r');
    uart_puts_p(PSTR("size:   ")); uart_putdw_dec(disk_info.capacity / 1024 / 1024); uart_puts_p(PSTR("MB\n"));
    uart_puts_p(PSTR("copy:   ")); uart_putw_dec(disk_info.flag_copy); uart_putc('\n');uart_putc('\r');
    uart_puts_p(PSTR("wr.pr.: ")); uart_putw_dec(disk_info.flag_write_protect_temp); uart_putc('/');
                                   uart_putw_dec(disk_info.flag_write_protect); uart_putc('\n');uart_putc('\r');
    uart_puts_p(PSTR("format: ")); uart_putw_dec(disk_info.format); uart_putc('\n');uart_putc('\r');
    //uart_puts_p(PSTR("free:   ")); uart_putdw_dec(fat_get_fs_free(fs)); uart_putc('/');
                                   //uart_putdw_dec(fat_get_fs_size(fs)); uart_putc('\n');uart_putc('\r');

    return 1;
}
