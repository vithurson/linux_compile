/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2008 David Gibson, IBM Corporation
 * Copyright (C) 2012 Regents of the University of California
 * Copyright (C) 2017 SiFive
 */

#include <linux/console.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/types.h>


#include "hvc_console.h"

static int hvc_sbi_tty_put(uint32_t vtermno, const char *buf, int count)
{
	int i;

	for (i = 0; i < count; i++){
         volatile int *x = (int *)0xe000002c;
         while ((*x&16)==16);
         *(int*) 0xe0000030= buf[i];
    }

	return i;
}

static int hvc_sbi_tty_get(uint32_t vtermno, char *buf, int count)
{
	int i;
	for (i = 0; i < count; i++) {
        volatile int *c = (int*) 0xe0000030;
        volatile int *x = (int*) 0xe000002c;
		if((*x&0x2)==2){                     //checking the data available bit in line status reg
            break;
		}
		buf[i] = *c;
	}

	return i;
}

static const struct hv_ops hvc_sbi_ops = {
	.get_chars = hvc_sbi_tty_get,
	.put_chars = hvc_sbi_tty_put,
};

static int __init hvc_sbi_init(void)
{
	return PTR_ERR_OR_ZERO(hvc_alloc(0, 0, &hvc_sbi_ops, 16));
}
device_initcall(hvc_sbi_init);

static int __init hvc_sbi_console_init(void)
{
	hvc_instantiate(0, 0, &hvc_sbi_ops);

	return 0;
}
console_initcall(hvc_sbi_console_init);
