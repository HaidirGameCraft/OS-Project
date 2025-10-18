#pragma once

#include "boot_header.h"
#include "page.h"
#include "memory.h"
#include "fs.h"

void Initialize(struct boot_header* bootheader);
void printf(const char* fmt, ...);