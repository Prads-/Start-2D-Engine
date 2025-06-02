#ifndef DECLARATIONS_H
#define DECLARATIONS_H

#include "build.h"

typedef unsigned char uint8;
typedef char int8;
typedef unsigned short int uint16;
typedef short int int16;
typedef unsigned int uint32;
typedef int int32;
typedef long long int64;
typedef unsigned long long uint64;

typedef float float32;

#define MAXIMUM_NUMBER_OF_CLIENTS		4
#define BALL_CATCHABLE_SPEED			2.0f

#define _FLOAT_PI			3.141593f
#define _WINSOCKAPI_

#ifdef BUILD_FOR_WINDOWS
#define KEY_UP				0x26
#define KEY_DOWN			0x28
#define KEY_LEFT			0x25
#define KEY_RIGHT			0x27
#define KEY_BACKSPACE		8
#define KEY_SHIFT			16
#define KEY_ESCAPE			27
#define KEY_DELETE			46
#define KEY_DECIMAL			190
#else
#define KEY_UP				265
#define KEY_DOWN			264
#define KEY_LEFT			263
#define KEY_RIGHT			262
#define KEY_BACKSPACE		259
#define KEY_SHIFT			340
#define KEY_ESCAPE			256
#define KEY_DELETE			261
#define KEY_DECIMAL			330
#endif

#define BATTERY_SPEED_UP	1.75f

#define D3D_DEBUG_INFO

#endif