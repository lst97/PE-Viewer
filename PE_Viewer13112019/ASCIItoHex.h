#pragma once
#ifndef ASCIITOHEX_H_
#define ASCIITOHEX_H_

#define IN
#define OUT

#include "stdio.h"
#include "string.h"

bool ASCIItoHex(IN unsigned char* szString, OUT unsigned char* szBuffer, int size);
#endif
