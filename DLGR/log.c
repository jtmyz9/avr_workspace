/*
 * log.c
 *
 *  Created on: Mar 27, 2012
 *      Author: Administrator
 */

#include "log.h"
#include "log_structure.h"
#include <stdio.h>

volatile LOG_buf_t log_buf;
volatile uint32_t log_timer = 0;
volatile LOG_t sys;
struct fat_file_struct* f_log;
volatile uint8_t log_status = NOT_LOGGING;

struct partition_struct* partition;
struct fat_fs_struct* fsys;
struct fat_dir_entry_struct dir_ent;
struct fat_dir_struct* dd;

/* open root directory */
struct fat_dir_entry_struct directory,log_dir;
struct fat_dir_entry_struct new_dir;



char fat_init(void)
{
	if(!sd_raw_init())
	{
		uart_puts_p("Raw SD Init Failed\n");
		return -1;
	}
	uart_puts_p(PSTR("Raw SD Init succeeded\n"));

	partition =	partition_open(sd_raw_read,sd_raw_read_interval,sd_raw_write,sd_raw_write_interval,0);

	if(!partition)
	{
		uart_puts_p(PSTR("First partition Open failed, trying something else\n"));
		partition = partition_open(sd_raw_read,sd_raw_read_interval,sd_raw_write,sd_raw_write_interval,-1);
		if(!partition)	return(-2);
	}

	uart_puts_p(PSTR("Partition Opened\n"));

	fsys = fat_open(partition);

	if(!fsys)
	{
		uart_puts_p(PSTR("Opening filesystem Failed\n"));
		return(-3);
	}

	uart_puts_p(PSTR("Filesystem Opened\n"));

	return(0);

}



uint8_t _log(uint8_t *blk)
{
	uint16_t size; // Size of block to be logged (comes from the block header info
	uint16_t idx = 0; // Current Block Index
	LOG_hdr_t *hdr = (LOG_hdr_t *) blk;		// Cast to Log header pointer to manipulate header info

	// Set the current timer log
	hdr->time = log_timer;

	// Get the block size
	size = hdr->size;

	if(f_log != 0)
	{
		while(idx < size)
		{
			if(!log_space())
			{
				flush_log();
			}
			else
			{
				log_buf.data[log_buf.next] = blk[idx];
				log_buf.next++;
				idx++;
			}
		}

		return 1;
	}
	else
	{
		return 0;
	}
}

void close_log(void)
{
	uint8_t eof[4] = {LOG_EOF,LOG_EOF,LOG_EOF,LOG_EOF};



	flush_log();
	fat_write_file(f_log,eof,4);
	fat_close_file(f_log);
	//fat_close_dir(dd);
	//fat_close(fsys);
	sd_raw_sync();
	log_status = NOT_LOGGING;
}

void flush_log(void)
{
	PORTE |= _BV(PE6);
	fat_write_file(f_log,(uint8_t *)log_buf.data,log_buf.next);
	log_buf.next = 0;
	PORTE &= ~_BV(PE6);

#ifdef CLEAR_LOG_ON_FLUSH
	clear_log();
#endif
}

uint8_t open_log(const char *path)
{
	char file_str[20] = "";
	uint8_t file_num = 0;
	uint8_t resp = 0;

    sprintf(file_str,"%s%u.log",path,file_num);

    uart_puts_p(PSTR("Opening New log\n"));

	log_timer = 0;
	clear_log();
    do
    {
    	resp = fat_create_file(dd,file_str,&directory);
    	if(resp == 0)
    	{
    		uart_puts_p(PSTR("Cannot Open File\n"));
    		return 0;
    	}
    	if(resp == 2)
    	{
    		file_num++;
    	    sprintf(file_str,"%s%u.log",path,file_num);
    	}
    }
    while(resp != 1);

    uart_puts_p(PSTR("File Created\n"));

    f_log = fat_open_file(fsys,&directory);
    if(!f_log)
    {
    	uart_puts_p(PSTR("Creating File Opened\n"));
    	return 0;
    }

	log(sys.log.prm);
	log_status = LOGGING;
	return 1;

}

void print_log_buf(void)
{
	uint16_t i,j;

	for(i = 0;i<32;i++)
	{
		for(j=0;j<32;j++)
		{
			uart_putc_hex(log_buf.data[i*32+j]);
			uart_putc(' ');
		}
		uart_putc('\n');
	}
}

void clear_log(void)
{
	uint16_t i;

	for(i = 0;i<log_buf.size;i++)
	{
		log_buf.data[i] = 0;
	}

	log_buf.next = 0;
}

uint8_t init_log_system(void)
{
	log_buf.size = LOG_BUF_SIZE;
	// Fat file system init should go here later
	sys.log.f100.hdr.blk_id = LOG_100HZ_ID;
	sys.log.f10.hdr.blk_id = LOG_10HZ_ID;
	sys.log.f5.hdr.blk_id = LOG_5HZ_ID;
	sys.log.f1.hdr.blk_id = LOG_1HZ_ID;
	sys.log.prm.hdr.blk_id = LOG_PRM_ID;
	sys.log.status.hdr.blk_id = LOG_STATUS_ID;

	// Fat file system init should go here later
	sys.log.f100.hdr.size = sizeof(LOG_100HZ_t);
	sys.log.f10.hdr.size = sizeof(LOG_10HZ_t);
	sys.log.f5.hdr.size = sizeof(LOG_5HZ_t);
	sys.log.f1.hdr.size = sizeof(LOG_1HZ_t);
	sys.log.prm.hdr.size = sizeof(LOG_PRM_t);
	sys.log.status.hdr.size = sizeof(LOG_STATUS_t);

    sys.data.chan.fl_susp_trvl 	= 0;
    sys.data.chan.fr_susp_trvl	= 0;
    sys.data.chan.rl_susp_trvl	= 0;
    sys.data.chan.rr_susp_trvl	= 0;
    sys.data.chan.acl_x 		= 0;
    sys.data.chan.acl_y 		= 0;
    sys.data.chan.acl_z 		= 0;
    sys.data.chan.gyro_x 		= 0;
    sys.data.chan.gyro_y 		= 0;
    sys.data.chan.gyro_z 		= 0;

    sys.data.chan.rpm = 0;			// 10 Hz
    sys.data.chan.tps = 0;			// 10 Hz
    sys.data.chan.oil_pres = 0;		// 10 HZ
    sys.data.chan.fbrk_pres = 0;		// 10 HZ
    sys.data.chan.rbrk_pres = 0;		// 10 HZ
    sys.data.chan.fuel_open_time = 0;// 10 HZ
    sys.data.chan.ign_angle = 0;		// 10 HZ
    sys.data.chan.map = 0;			// 10 HZ
    sys.data.chan.lambda = 0;		// 10 HZ
    sys.data.chan.fuel_slip_err = 0;	// 10 HZ
    sys.data.chan.fuel_comp = 0;	// 10 HZ
    sys.data.chan.timing_err = 0;	// 10 HZ
    sys.data.chan.timing_comp = 0;	// 10 HZ
    sys.data.chan.fl_wspd = 0;		// 10 HZ
    sys.data.chan.fr_wspd = 0;		// 10 HZ
    sys.data.chan.rl_wspd = 0;		// 10 HZ
    sys.data.chan.rr_wspd = 0;		// 10 HZ
    sys.data.chan.lng_slip = 0;		// 10 HZ
    sys.data.chan.lat_slip = 0;		// 10 HZ
    sys.data.chan.sw_angle = 0;		// 10 HZ

    sys.data.chan.ttemp_fli = 0;	// 5HZ
    sys.data.chan.ttemp_flm = 0;	// 5HZ
    sys.data.chan.ttemp_flo = 0;	// 5HZ
    sys.data.chan.ttemp_fri = 0;	// 5HZ
    sys.data.chan.ttemp_frm = 0;	// 5HZ
    sys.data.chan.ttemp_fro = 0;	// 5HZ
    sys.data.chan.ttemp_rli = 0;	// 5HZ
    sys.data.chan.ttemp_rlm = 0;	// 5HZ
    sys.data.chan.ttemp_rlo = 0;	// 5HZ
    sys.data.chan.ttemp_rri = 0;	// 5HZ
    sys.data.chan.ttemp_rrm = 0;	// 5HZ
    sys.data.chan.ttemp_rro = 0;	// 5HZ
    sys.data.chan.lat = 0;			// 5HZ
    sys.data.chan.lng = 0;			// 5HZ
    sys.data.chan.heading = 0;	// 5HZ
    sys.data.chan.speed = 0;		// 5HZ

    sys.data.chan.batt_volt = 0;			// 1HZ
    sys.data.chan.air_temp  = 0;			// 1HZ
    sys.data.chan.wtr_temp  = 0;			// 1HZ
    sys.data.chan.oil_temp  = 0;			// 1HZ

	// Log info
	sys.data.prm.log_num = 0;				// Parameter
	sys.data.prm.driver = 0;		// Parameter
	sys.data.prm.date = 0;			// Parameter
	sys.data.prm.time = 0;
	sys.data.prm.hmsphr = 0;

	// ECU info
	sys.data.prm.ver_mjr = 0;		// Parameter
	sys.data.prm.ver_min = 0;		// Parameter
	sys.data.prm.ver_bld = 0;		// Parameter
	sys.data.prm.temp_type = 0;		// Parameter
	sys.data.prm.pres_type = 0;		// Parameter

	// Traction Control Parameters
	sys.data.prm.fuel_kp  = 0;		// Parameter
	sys.data.prm.fuel_kd = 0;		// Parameter
	sys.data.prm.fuel_ki = 0;		// Parameter
	sys.data.prm.timing_kp = 0;		// Parameter
	sys.data.prm.timing_kd = 0;		// Parameter
	sys.data.prm.timing_ki = 0;		// Parameter
	sys.data.prm.tcs_slip_pct = 0;	// Parameter
	sys.data.prm.lnc_dsb_spd = 0;	// Parameter

	// Accelerometer Parameters
	sys.data.prm.x_acl_comp = 0;		// Parameter
	sys.data.prm.y_acl_comp = 0;		// Parameter
	sys.data.prm.z_acl_comp = 0;		// Parameter

	sys.data.chan.curr_tick = TCNT3;
	sys.data.chan.last_tick = sys.data.chan.curr_tick;

	if(fat_init()!=0)
	{
		uart_puts_p(PSTR("FAT Error\n"));
		return 0;
	}

    if(!fat_get_dir_entry_of_path(fsys,"/",&directory))
    {
    	uart_puts_p(PSTR("Get path failed\n"));
    	return 0;
    }

    dd = fat_open_dir(fsys, &directory);
    if(!dd)
    {
        uart_puts_p(PSTR("opening root directory failed\n"));
        while(1);
    }

	return 1;
}
