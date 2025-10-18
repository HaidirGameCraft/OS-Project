#pragma once

#include <stdint.h>

/**
 * inb()
 * - get the u8 data from specific port
 */
extern u8 inb(u16 port);

/**
 * inw()
 * - get the u16 data from specific port
 */
extern u16 inw(u16 port);

/**
 * outb()
 * - set the data inside of specific port
 */
extern void outb(u16 port, u8 data);

/**
 * outw()
 * - set the data inside of specific port
 */
extern void outw(u16 port, u16 data);
