#pragma once 
typedef unsigned char 		uint8;
typedef unsigned short 		uint16;
typedef unsigned int 		uint;
typedef unsigned int		uint32;
typedef unsigned __int64 	uint64;
typedef char 				int8;
typedef short 				int16;
typedef int					int32;
typedef __int64 			int64;

#define in
#define out
#define inout
#define is ==
#define null nullptr
#define interface struct
#define class struct
#define cast reinterpret_cast
#define mixin
#define funcdef
inline static void super(...) {}

#ifdef _WIN64
#include "api64.h"
#else
#include "api.h"
#endif

#include "src/main.as"
#include "src/ipc/ipc.as"
