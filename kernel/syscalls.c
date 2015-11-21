/** @file syscalls.c
 *
 *  @brief This file contains the functionality of the syscalls
 *
 *  @author Alejandro Jove (ajovedel)
 *  @author Vishnu Gorantla (vishnupg)
 *  @bug No known bugs
 */
#include <exports.h>
#include <bits/errno.h>
#include <bits/fileno.h>
#include <arm/reg.h>
#include <arm/timer.h>
#include "syscalls.h"
#include "globals.h"

#define BACKSPACE '\b'
#define DELETE    127
#define EOT       4
#define NL        10
#define CR        13

#define RAM_START 0xA0000000
#define RAM_END   0xA3FFFFFF
#define ROM_END   0xFFFFFF

/** @brief Read syscall
 *
 *  Read N character from STDIN and place it in a buffer
 *  @param  fd  The file descriptor to read from
 *  @param  buf The data buffer to store the read data
 *  @param  count The number of bytes to read
 *  @return The number of bytes read on success; otherwise -1
 */
ssize_t read(int fd, void *buf, size_t count){
	
	unsigned i;
	unsigned char temp;

	// fd has to be STDIN, otherwise return bad fd
	if(fd != STDIN_FILENO)
		return -EBADF;

  unsigned start_buffer = (unsigned)buf;
  unsigned end_buffer = start_buffer + count;

	// verify that buf and buf+count is in range of SDRAM (A0000000-A3FFFFFF)
	if(!(start_buffer > RAM_START && start_buffer < RAM_END))
		return -EFAULT;
	
  if(!(end_buffer > RAM_START && end_buffer < RAM_END))
		return -EFAULT;
	
	for(i=0; i<count; i++){

		// get the character from STDIN
		temp = getc();

		// check for EOT character
		if(temp == EOT)
			return ++i;

		// check for backspace or delete
		else if(temp == BACKSPACE || temp == DELETE)
			puts("\b \b");
			
		// check for newline and carriage return
		else if(temp == CR || temp == NL){
			((char *)buf)[i] = '\n';
			putc('\n');
			return ++i;
		}

		// if not a special char, read to buffer and write to STDOUT
		else{
			((char *)buf)[i] = temp;
			putc(temp);
		}
	}
	return i;
}

/** @brief Write system call
 *  
 *  Reads N characters from a buffer and place it in STDOUT 
 *  @param fd File Descriptor to write to
 *  @param buf The buffer to write data from 
 *  @param count The maximum size to write the data
 *  @return The number of bytes written
 */
ssize_t write(int fd, const void *buf, size_t count){
	
	unsigned i;

	// fd has to be STDOUT, otherwise return bad fd
	if (fd != STDOUT_FILENO)
		return -EBADF;

  unsigned start_buffer = (unsigned)buf;
  unsigned end_buffer = start_buffer + count;

	// verify that buf and buf+count is in range of SDRAM (A0000000-A3FFFFFF)
	// or StrataFlash ROM (0 - 00FFFFFF)
	if(! ((start_buffer > RAM_START && start_buffer < RAM_END) || 
                          (start_buffer < ROM_END)) )
		return -EFAULT;

	if(! ((end_buffer > RAM_START && end_buffer < RAM_END) || 
                          (end_buffer < ROM_END)) )
		return -EFAULT;

	// write buffer to STDOUT
	for(i=0; i<count; i++){
    
    char buf_char = ((char *)buf)[i];
		// return if null is reached
		if(buf_char == '\0')
			return i;

		putc(buf_char);
	}
	return i;
}


/** @brief Time system call
 *  
 *  Get the time in miliseconds since the kernel booted up 
 *  @return The current time since system booted tup, in milliseconds
 */
unsigned long time(){
	return (TIME_UNITS_ELAPSED * TIME_RES_MS + 
           ((reg_read(OSTMR_OSCR_ADDR) / OS_CLK_SPEED) % TIME_RES_MS));
}


/** @brief Sleep system call
 *  Halts execution until the time has elapsed 
 *
 *  @return None.
 */
void sleep(unsigned long sleep_time){

	unsigned long start_time, end_time;	

	// get current OSCR
	start_time = reg_read(OSTMR_OSCR_ADDR);

	// calculate time we sleep
	end_time = start_time + (sleep_time * (TIME_RES_CYCLES * 0.1));

	// sleep
	while(reg_read(OSTMR_OSCR_ADDR) < end_time); 
}
