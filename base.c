#include "base.h"

u8 keydata;
u8 *vram = VRAM;

static u8 frameCount;

void bzero(char *p, int size) {
	while (size-- > 0) *p++ = 0;
}

void cls(void) {
	bzero(vram, XN * YN);
}

void clrs(void) {
	bzero(vram + 18 * XN, XN * 136);
}

void printf(const char *format, ...) {
	uint *ap = (uint *)&format;
	u8 *p = (u8 *)format;
#ifdef __CHIBICC__
	ap += 2;
#endif
#ifdef STDARG_REV
#define next_ap	(--ap)
#else
#define next_ap	(++ap)
#endif
	while (*p) {
		u8 c[12]; // for 32bit
		u8 a, b, f, i, l, n, r, t, u;
		uint v;
		u8 *q;
		switch (t = *p++) {
		case '%':
			b = ' ';
			l = n = 0;
			switch (t = *p++) {
				case '%': putchar('%'); continue;
				case '0': b = '0'; t = *p++; break;
				case '-': l = 1; t = *p++; break;
				case 'c': putchar(*next_ap); continue;
			}
			if (t >= '1' && t <= '9')
				for (n = t - '0'; (t = *p++) >= '0' && t <= '9';)
					n = 10 * n + t - '0';
			if (t == 's')
				for (q = *(u8 **)next_ap, i = 0; q[i]; i++)
					;
			else {
				if (u = t == 'u') t = *p++;
				switch (t) {
					case 'd': r = 10; break;
					case 'o': r = 8; u = 1; break;
					case 'x': r = 16; u = 1; a = 0x27; break;
					case 'X': r = 16; u = 1; a = 7; break;
					default: continue;
				}
				v = *next_ap;
				if (f = !u && (s16)v < 0) v = -v;
				c[i = sizeof(c) - 1] = 0;
				do {
					uint d = v / r, t1 = v - d * r + '0';
					c[--i] = t1 > '9' ? t1 + a : t1;
					v = d;
				} while (v);
				if (f) c[--i] = '-';
				q = &c[i];
			}
			i = sizeof(c) - 1 - i;
			if (l) n -= i;
			else while (n-- > i) putchar(b);
			while (*q) putchar(*q++);
			if (l) while (n--) putchar(' ');
			break;
		default:
			putchar(t);
			break;
		}
	}
#undef next_ap
}

static u32 rndv = 12345;

u16 rand(void) { // xorshift
	rndv = rndv ^ (rndv << 13);
	rndv = rndv ^ (rndv >> 17);
	rndv = rndv ^ (rndv << 5);
	return (u16)(rndv & 0x7fff);
}

u16 R(u16 n) {
	return (u16)((u32)n * rand() >> 15);
}

#ifdef KP
u8 interruptCount;
void interrupt1(void) {
	interruptCount++;
}
#endif

void waitVSync(u8 frames) {
	u8 dif = (u8)interruptCount - frameCount;
	u8 f, t = frameCount + frames;
	if (t >= frameCount)
		while ((f = interruptCount) >= frameCount && f < t)
			wait();
	else
		while ((f = interruptCount) >= frameCount || f < t)
			wait();
	frameCount = f;
}

void vramSwap(u8 frames) {
	u8 f = vram == VRAM;
	waitVSync(frames);
	vram = f ? VRAM2 : VRAM;
	VRAMADR = (u16)(f ? VRAM : VRAM2) >> 8;
}

void vramSingle(void) {
	vram = VRAM;
	VRAMADR = (u16)VRAM >> 8;
}
