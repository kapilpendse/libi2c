#ifndef _LIBI2C_H_
#define _LIBI2C_H_

/*
 * Write data to I2C bus.
 * devnode - The I2C bus device node e.g. "/dev/i2c-0"
 * addr - The 7-bit I2C address of destination device.
 * w    - Pointer to array of data to be written.
 * wlen - Number of bytes to write from the array 'w' to the I2C bus.
 * On success, returns the number of bytes written. Returns -1 otherwise.
 */
int li2c_write(const char *devnode,
               const unsigned char addr,
               const unsigned char *w,
               const unsigned wlen);

/*
 * Read data from I2C bus.
 * devnode - The I2C bus device node e.g. "/dev/i2c-0"
 * addr - The 7-bit I2C address of device to read from.
 * r    - Pointer to array where received bytes will be stored.
 * rlen - Number of bytes to read from the I2C bus to the array 'r'.
 *        User must ensure that the array 'r' has at least 'rlen' bytes
 *        allocated.
 * On success, returns the number of bytes read. Returns -1 otherwise.
 */
int li2c_read(const char *devnode,
              const unsigned char addr,
              unsigned char *r,
              unsigned rlen);

/*
 * Write data, then read data.
 * devnode - The I2C bus device node e.g. "/dev/i2c-0"
 * addr - The 7-bit I2C address of destination device.
 * w    - Pointer to array of data to be writen
 * wlen - Number of bytes to write from the array 'w' to the I2C bus.
 * r    - Pointer to array where received bytes will be stored.
 * rlen - Number of bytes to read from the I2C bus to the array 'r'.
 *        User must ensure that the array 'r' has at least 'rlen' bytes
 *        allocated.
 * stop - Boolean flag, 0 means there will be no STOP condition between the
 *        write and the read; 1 means there will be a STOP condition between
 *        the write and the read.
 * On success, returns the number of bytes read. Returns -1 otherwise.
 */
int li2c_writeread(const char *devnode,
                   const unsigned char addr,
                   const unsigned char *w,
                   const unsigned wlen,
                   unsigned char *r,
                   unsigned rlen,
                   unsigned char stop);

#endif /*_LIBI2C_H_*/

