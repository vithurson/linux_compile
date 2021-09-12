/*
 * TinyEMU
 * 
 * Copyright (c) 2016-2018 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __TEMU_H_
#define __TEMU_H_


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#ifndef _WIN32
#include <termios.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_tun.h>
#endif
#include <sys/stat.h>
#include <signal.h>

#include "cutils.h"
// #include "../emu.h"
#include "iomem.h"
#include "virtio.h"
#include "machine.h"
#ifdef CONFIG_FS_NET
#include "fs_utils.h"
#include "fs_wget.h"
#endif
#ifdef CONFIG_SLIRP
#include "slirp/libslirp.h"
#endif

#ifndef _WIN32

typedef struct {
    int stdin_fd;
    int console_esc_state;
    BOOL resize_pending;
} STDIODevice;

struct termios oldtty;
int old_fd0_flags;
STDIODevice *global_stdio_device;

void term_exit(void);


void term_init(BOOL);


void console_write(void *, const uint8_t *, int );


int console_read(void *, uint8_t *, int );


void term_resize_handler(int );

void console_get_size(STDIODevice *, int *, int *);


CharacterDevice *console_init(BOOL );

#endif /* !_WIN32 */

typedef enum {
    BF_MODE_RO,
    BF_MODE_RW,
    BF_MODE_SNAPSHOT,
} BlockDeviceModeEnum;

#define SECTOR_SIZE 512

typedef struct BlockDeviceFile {
    FILE *f;
    int64_t nb_sectors;
    BlockDeviceModeEnum mode;
    uint8_t **sector_table;
} BlockDeviceFile;

int64_t bf_get_sector_count(BlockDevice *);


//#define DUMP_BLOCK_READ

int bf_read_async(BlockDevice *,
                         uint64_t , uint8_t *, int ,
                         BlockDeviceCompletionFunc *, void *);


int bf_write_async(BlockDevice *,
                          uint64_t , const uint8_t *, int ,
                          BlockDeviceCompletionFunc *, void *);


BlockDevice *block_device_init(const char *,BlockDeviceModeEnum );


#ifndef _WIN32

typedef struct {
    int fd;
    BOOL select_filled;
} TunState;

void tun_write_packet(EthernetDevice *,
                             const uint8_t *, int );


void tun_select_fill(EthernetDevice *, int *,
                            fd_set *, fd_set *, fd_set *,
                            int *);


void tun_select_poll(EthernetDevice *, 
                            fd_set *, fd_set *, fd_set *,
                            int );


/* configure with:
# bridge configuration (connect tap0 to bridge interface br0)
   ip link add br0 type bridge
   ip tuntap add dev tap0 mode tap [user x] [group x]
   ip link set tap0 master br0
   ip link set dev br0 up
   ip link set dev tap0 up

# NAT configuration (eth1 is the interface connected to internet)
   ifconfig br0 192.168.3.1
   echo 1 > /proc/sys/net/ipv4/ip_forward
   iptables -D FORWARD 1
   iptables -t nat -A POSTROUTING -o eth1 -j MASQUERADE

   In the VM:
   ifconfig eth0 192.168.3.2
   route add -net 0.0.0.0 netmask 0.0.0.0 gw 192.168.3.1
*/
EthernetDevice *tun_open(const char *);


#endif /* !_WIN32 */

#ifdef CONFIG_SLIRP

/*******************************************************/
/* slirp */

Slirp *slirp_state;

void slirp_write_packet(EthernetDevice *,
                               const uint8_t *, int );


int slirp_can_output(void *);


void slirp_output(void *, const uint8_t *, int );


void slirp_select_fill1(EthernetDevice *, int *,
                               fd_set *, fd_set *, fd_set *,
                               int *);


void slirp_select_poll1(EthernetDevice *, 
                               fd_set *, fd_set *, fd_set *,
                               int );


EthernetDevice *slirp_open(void);


#endif /* CONFIG_SLIRP */

#define MAX_EXEC_CYCLE 500000
#define MAX_SLEEP_TIME 10 /* in ms */

void virt_machine_run(VirtMachine *);


/*******************************************************/

struct option static options[] = {
    { "help", no_argument, NULL, 'h' },
    { "ctrlc", no_argument },
    { "rw", no_argument },
    { "ro", no_argument },
    { "append", required_argument },
    { "no-accel", no_argument },
    { "build-preload", required_argument },
    { NULL },
};

void help(void);


#ifdef CONFIG_FS_NET
BOOL net_completed;

void net_start_cb(void *);


BOOL net_poll_cb(void *);


#endif

int temu_main(int , char **);


#endif