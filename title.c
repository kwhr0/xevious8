#include "title.h"
#include "base.h"
#include "chr.h"
#include "game.h"
#include "sprite.h"
#include "pattern.h"
#include "music.h"
#include "play.h"

static u8 behavior0(Sprite *p) {
	return p->animSpeed != 0;
}

static u8 behavior1(Sprite *p) {
	if (p->y >= 83 << PS) {
		spriteAnim(p, pat_tss);
		p->behavior = behavior0;
		p->speedY = 0;
		p->flags = 0;
		spriteFrame(p, 0x7fff);
		p->animSpeed = -1 << AS;
	}
	return 1;
}

static u8 behavior2(Sprite *p) {
	if (!p->animSpeed) {
		spriteAnim(p, pat_tsr);
		p->behavior = behavior1;
		p->speedY = 1 << PS;
		p->flags = SF_LOOP;
	}
	return 1;
}

static void generate(void) {
	Sprite *p = spriteCreate(0, pat_tss);
	if (!p) return;
	p->behavior = behavior2;
	setX(p, 108);
	setY(p, 22);
}

void titleMain(u8 f) {
	static Sprite sprite;
	u8 i;
	spriteSetup(&sprite, 1, 0);
	cls();
	vramSwap(0);
	cls();
	chrInit();
	scorePrint();
	chrUpdate();
	vramSwap(0);
	chrFlush();
	chrTime(2);
	chrLocate(3, 30);
	if (f) {
		playMute(0);
		playStart(0, MUSIC_CREDIT, 0);
	}
	else chrPuts("PUSH O BUTTON");
	logoInit();
	for (i = 0; i < (f ? 20 : 100); i++) {
		u8 *p = vram + XN * 46;
		while (p < vram + XN * 52) *p++ = 0;
		if (!f && i == 5) generate();
		chrUpdate();
		spriteUpdate();
		logoUpdate();
		if (!f && KEY & KEY_A) {
			demoSet(0);
			return;
		}
		vramSwap(3);
	}
}
