#include "types.h"

#define putchar(c)		(PRINTER = c)

#define XN				12
#define YN				160

#ifdef __CHIBICC__
#define sleep()	asm(".byte 0x1a")
#define wait()	asm("wai")
#define ei()	asm("cli")
#define di()	asm("sei")
#elif defined(KP)
#define sleep()
#define wait()
#define ei()
#define di()
#else
void sleep(void);
void wait(void);
void ei(void);
void di(void);
#endif

#define lf	putchar('\n')

extern u8 keydata, interruptCount;
extern u8 *vram;

void bzero(char *, int);
void cls(void);
void clrs(void);
void printf(const char *format, ...);
u16 rand(void);
u16 R(u16 n);
void waitVSync(u8 frames);
void vramSwap(u8 frames);
void vramSingle(void);

// for 6502
void puts_n(char *s);
char sr4(char v);
void put2(u8 v);
void put4(u16 v);
void put4s(char *s, u16 v, u8 f);
