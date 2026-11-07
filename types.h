#ifndef _TYPES_H_
#define _TYPES_H_

#ifndef nil
#define nil	0
#endif

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned int uint;

#define KEY_UP			1
#define KEY_DOWN		2
#define KEY_LEFT		4
#define KEY_RIGHT		8
#define KEY_A			16
#define KEY_B			32

#ifdef KP
#define VRAM	((u8 *)0x49000)
#define VRAM2	((u8 *)0x49800)
#define PRINTER	(*(u8 *)0x4eff0)
#define VRAMADR	(*(u8 *)0x4eff1)
#define SN		(*(u8 *)0x4eff2)
#define KEY	(*(u8 *)0x4eff0)
#else
#ifdef INS8070
#define VRAM	((u8 *)0xe000)
#define VRAM2	((u8 *)0xe800)
#else
#define VRAM	((u8 *)0xf000)
#define VRAM2	((u8 *)0xf800)
#endif
#define PRINTER	(*(u8 *)0xeff0)
#define VRAMADR	(*(u8 *)0xeff1)
#define SN		(*(u8 *)0xeff2)
#define KEY	(*(u8 *)0xeff0)
#endif

#endif
