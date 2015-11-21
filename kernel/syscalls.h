/** @file syscalls.h
 *
 *  @brief This file contains the prototypes for the syscalls
 *
 *  @authoe Alejandro Jove(ajovedel)
 *  @author Vishnu Gorantla (vishnupg)
 *  @bug No known bugs
 */

ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
unsigned long time();
void sleep(unsigned long sleep_time);
