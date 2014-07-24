#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <string.h>
#include "i2c-dev.h"
#include "libi2c.h"


/***** Internal functions *****/

/*
 * Opens the specified I2C bus device node.
 * devnode - e.g. /dev/i2c-0.
 * fd      - Pointer to hold the opened file descriptor.
 * Returns 0 on success, -1 otherwise. Error messages are printed by perror().
 */
static int init_fd(const char *devnode, int *fd) {
    //sanity check
    if(devnode == NULL || fd == NULL) {
        return -1;
    }

    //open fd
    if((*fd = open(devnode, O_RDWR)) < 0) {
        perror(devnode);
        return -1; //failure
    }

    //success
    return 0;
}

/*
 * Closes the I2C bus device node which was previously opened by init_fd().
 * fd      - The opened I2C bus file descriptor to close.
 * Returns 0 on success, -1 otherwise. Error messages are printed by perror().
 */
static int deinit_fd(const int fd) {
    //sanity check
    if(fd < -1) {
        return -1;
    }

    //close fd
    if(close(fd) < 0) {
        perror("deinit_fd");
        return -1; //failure
    }

    //success
    return 0;
}

/*
 * Sets the specified I2C address to be used for the following
 * read/write operations.
 * fd      - The opened I2C bus file descriptor.
 * addr    - The 7-bit I2C address of destination device.
 * Returns 0 on success, -1 otherwise. Error messages are printed by perror().
 */
static int set_slave(const int fd, const unsigned char addr) {
    //sanity check
    if(fd < 0) {
        return -1;
    }

    //set slave address
	if(ioctl(fd, I2C_SLAVE_FORCE, addr) < 0) {
		perror("set_slave");
		return -1; //failure
	}

    //success
    return 0;
}


/***** Public functions *****/

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
               const unsigned wlen) {

    int fd = -1;
    int bwrote = 0;

    //sanity check
    if(devnode == NULL || w == NULL || wlen == 0) {
        fprintf(stderr, "li2c_write: Invalid parameters\n");
        return -1;
    }

    //initialize I2C bus
    if(init_fd(devnode, &fd) < 0) {
        fprintf(stderr, "li2c_write: Failed to initialize I2C bus.\n");
        return -1;
    }

    //set slave address
    if(set_slave(fd, addr) < 0) {
        fprintf(stderr, "li2c_write: Failed to reach device 0x%X\n.", addr);
        deinit_fd(fd);
        return -1;
    }

    //write data to I2C bus
    bwrote = write(fd, w, wlen);
    deinit_fd(fd);
    if(bwrote <= 0) {
        fprintf(stderr, "li2c_write: Fail to write data to I2C bus.\n");
        return -1;
    } else {
        return bwrote;
    }
}

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
              unsigned rlen) {

    int fd = -1;
    int bread = 0;

    //sanity check
    if(devnode == NULL || r == NULL || rlen == 0) {
        fprintf(stderr, "li2c_read: Invalid parameters\n");
        return -1;
    }

    //initialize I2C bus
    if(init_fd(devnode, &fd) < 0) {
        fprintf(stderr, "li2c_read: Failed to initialize I2C bus.\n");
        return -1;
    }

    //set slave address
    if(set_slave(fd, addr) < 0) {
        fprintf(stderr, "li2c_read: Failed to reach device 0x%X\n.", addr);
        deinit_fd(fd);
        return -1;
    }

    //read data from I2C bus
    bread = read(fd, r, rlen);
    deinit_fd(fd);
    if(bread <= 0) {
        fprintf(stderr, "li2c_read: Fail to read data from I2C bus.\n");
        return -1;
    } else {
        return bread;
    }
}

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
                   unsigned char stop) {

    int fd = -1;
    int bwrote = 0, bread = 0;

    //sanity check
    if(devnode == NULL ||
       w == NULL ||
       wlen == 0 ||
       r == NULL ||
       rlen == 0) {
        fprintf(stderr, "li2c_writeread: Invalid parameters\n");
        return -1;
    }

    //unsupported operation check
    if(stop == 0) {
        fprintf(stderr,
                "li2c_writeread: write & read w/o STOP condition in between"
                " is not yet supported");
        return -1;
    }

    //initialize I2C bus
    if(init_fd(devnode, &fd) < 0) {
        fprintf(stderr, "li2c_writeread: Failed to initialize I2C bus.\n");
        return -1;
    }

    //set slave address
    if(set_slave(fd, addr) < 0) {
        fprintf(stderr, "li2c_writeread: Failed to reach device 0x%X\n.", addr);
        deinit_fd(fd);
        return -1;
    }

    //write data to I2C bus
    bwrote = write(fd, w, wlen);
    if(bwrote <= 0) {
        fprintf(stderr, "li2c_writeread: Failed to write data to I2C bus.\n");
        return -1;
    } else if(bwrote < wlen) {
        fprintf(stderr,
                "li2c_writeread: Incomplete write (requested %d, wrote %d).\n",
                wlen,
                bwrote);
        return -1;
    }

    //read data from I2C bus
    bread = read(fd, r, rlen);
    deinit_fd(fd);
    if(bread <= 0) {
        fprintf(stderr, "li2c_writeread: Failed to read data from I2C bus.\n");
        return -1;
    } else if(bread < rlen) {
        fprintf(stderr,
                "li2c_writeread: Incomplete read (requested %d, read %d).\n",
                rlen,
                bread);
        return -1;
    } else {
        return bread;
    }
}

