/** @file io.c
 *
 * @brief Kernel I/O syscall implementations
 *
 * @author Mike Kasick <mkasick@andrew.cmu.edu>
 * @date   Sun, 14 Oct 2007 00:07:38 -0400
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-11-16
 */

#include <types.h>
#include <bits/errno.h>
#include <bits/fileno.h>
#include <arm/physmem.h>
#include <syscall.h>
#include <exports.h>
#include <kernel.h>

#define BACKSPACE '\b'
#define DELETE    127
#define EOT       4
#define NL        10
#define CR        13

#define RAM_START 0xA0000000
#define RAM_END   0xA3FFFFFF
#define ROM_END   0xFFFFFF

/* Read count bytes (or less) from fd into the buffer buf. */
ssize_t read(int fd __attribute__((unused)), void *buf __attribute__((unused)), size_t count __attribute__((unused)))
{

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

/* Write count bytes to fd from the buffer buf. */
ssize_t write(int fd  __attribute__((unused)), const void *buf  __attribute__((unused)), size_t count  __attribute__((unused)))
{

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

