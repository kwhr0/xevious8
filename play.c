#include "play.h"
#include "base.h"

#define PLAYER_N	8

typedef struct {
	u8 *buf, *p;
	u16 wait;
	u8 ch, flags;
	u8 r[16];
	u8 env[4];
} Player;

Player player[PLAYER_N];
static u8 mute;

extern void (*interruptHandle)(void);
extern const u8 music[];

void playInit(void) {
	playStopAll();
	mute = 0;
}

#define INTERVAL	17
#define ENV		0xe8
#define SLEEP		0xee
#define TERM		0xef

static void playRestore(Player *c, u8 restore) {
	u8 *p = c->r;
	u8 m;
	for (m = 1; m < 16; p += 4, m <<= 1)
		if (restore & c->ch & m) {
			SN = p[0];
			SN = p[1];
			SN = p[2];
		}
}

static void playStopCh(u8 ch) {
	u8 m, d;
	for (m = 1, d = 0x9f; m < 16; m <<= 1, d += 0x20)
		if (ch & m) SN = d;
}

static void playInterrupt(void) {
	u8 restore = 0, higher_ch = 0;
	Player *c;
	for (c = player + PLAYER_N - 1; c >= player; c--) {
		if (!c->p) continue;
		if (restore) playRestore(c, restore);
		if (c->wait) {
			u8 i, j, b, m;
			s16 t = c->wait - INTERVAL;
			if (t < 0) t = 0;
			c->wait = t;
			for (i = 0, j = 2, b = 0x90, m = 1; i < 4; i++, j += 4, b += 0x20, m <<= 1) {
				u16 t1 = c->r[j + 1] + c->env[i];
				if (t1 > 0xff) t1 = 0xff;
				c->r[j + 1] = t1;
				t1 = b | t1 >> 4;
				c->r[j] = t1;
				if (!(higher_ch & m)) SN = t1;
			}
		}
		while (c->p && !c->wait) {
			u8 *p = c->p;
			u8 last = 0, valid = 0;
			while (*p != SLEEP && *p != TERM) {
				if (*p >= ENV && *p < ENV + 4) {
					c->env[*p - ENV] = p[1];
					p += 2;
				}
				else {
					u8 d = *p++;
					u8 sel = d >> 3 & 0xe;
					if (d & 0x80) {
						u8 m;
						c->r[sel] = d;
						last = sel;
						m = 1 << (sel >> 2);
						c->ch |= m;
						valid = !(higher_ch & m);
						if (d & 0x10) c->r[sel | 1] = d << 4;
					}
					else c->r[last | 1] = d;
					if (valid) SN = d;
				}
			}
			if (*p == SLEEP) {
				c->wait = p[1] | p[2] << 8;
				p += 3;
			}
			else if (c->flags & PF_LOOP) p = c->buf;
			else {
				p = nil;
				restore |= ~higher_ch & c->ch;
				c->ch = 0;
			}
			c->p = p;
		}
		higher_ch |= c->ch;
		restore &= ~c->ch;
	}
	playStopCh(restore);
}

void playStart(u8 index, u16 ofs, u8 f) {
	u8 i;
	Player *p;
	if (mute || index >= PLAYER_N) return;
	p = &player[index];
	di();
	if (p->p) playStop(index);
	ei();
	p->buf = (u8 *)&music[ofs];
	p->ch = 0;
	p->flags = f;
	p->wait = 0;
	for (i = 0; i < 16; i += 4) {
		p->r[i] = 0x8f | i << 3;
		p->r[i + 1] = 0;
		p->r[i + 2] = 0x9f | i << 3;
		p->r[i + 3] = 0xff;
	}
	for (i = 0; i < 4; i++) p->env[i] = 0;
	p->p = p->buf; // enable channel
//	interruptHandle = playInterrupt;
}

void playStop(u8 index) {
	u8 restore, higher_ch;
	Player *p, *c;
	if (index >= PLAYER_N) return;
	p = &player[index];
	restore = higher_ch = 0;
	di();
	for (c = player + PLAYER_N - 1; c >= player; c--)
		if (c->p) {
			playRestore(c, restore);
			if (c == p) {
				p->p = nil;
				restore = ~higher_ch & c->ch;
			}
			else {
				higher_ch |= c->ch;
				restore &= ~c->ch;
			}
		}
	ei();
	playStopCh(restore);
}

void playStopAll(void) {
	Player *c;
	di();
	for (c = player; c < player + PLAYER_N; c++) c->p = nil;
	ei();
	SN = 0x9f;
	SN = 0xbf;
	SN = 0xdf;
	SN = 0xff;
	interruptHandle = nil;
}

void playMute(u8 f) {
	mute = f;
	if (f) playStopAll();
}

u16 playing(u8 index) {
	Player *p;
	if (index >= PLAYER_N) return 0;
	p = &player[index];
	return p->p != nil ? p->buf - music : 0;
}
