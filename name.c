#include "name.h"
#include "base.h"
#include "chr.h"
#include "game.h"
#include "play.h"
#include "music.h"

typedef struct {
	u16 score;
	u8 name[9];
} Score;

static Score best[5] = {
	{ 4000, "NAKAMURA" },
	{ 3500, "EIRRY MO" },
	{ 3000, "EVZO END" },
	{ 2500, "OKAMOTO " },
	{ 2000, "S KOJIMA" },
};
static u8 pos;
static s8 edit = -1;

static void record(u8 index) {
	u8 buf[6];
	Score *p = &best[index];
	chrInit();
	chrTime(1);
	chrLocate(2, 25 + 3 * index);
	chrValue(buf, sizeof(buf), p->score);
	chrPuts(buf);
	chrPuts("0 ");
	chrPutsBuf(p->name);
	chrUpdate();
}

u8 nameEntry(u16 score) {
	s8 i, j;
	for (i = 0; i < 5 && score <= best[i].score; i++)
		;
	if (i < 5) {
		static const char def[] = "A       ";
		Score *sp, *dp;
		for (sp = &best[i], dp = &best[i + 1]; dp > best; --sp, --dp) {
			dp->score = sp->score;
			for (i = 0; i < 9; i++) dp->name[i] = sp->name[i];
		}
		best[i].score = score;
		for (j = 0; j < 9; j++) best[i].name[j] = def[j];
		edit = i;
		playStart(5, i ? MUSIC_NAME : MUSIC_BEST, PF_LOOP);
	}
	else edit = -1;
	return edit >= 0;
}

static void nameInit(void) {
	u8 i;
	pos = 0;
	chrTime(1);
	if (edit >= 0) {
		chrLocate(0, 12);
		chrPuts("CONGRATURATIONS!");
		chrFlush();
		chrLocate(1, 18);
		chrPuts("ENTER YOUR NAME");
	}
	else {
		logoInit();
		logoUpdate();
		chrLocate(1, 20);
		chrPuts("BEST 5 WARRIORS");
	}
	chrFlush();
	for (i = 0; i < 5; i++) record(i);
	if (edit >= 0) record(edit);
}

static u8 nameUpdate(void) {
	static u8 c0, timer;
	u8 c = KEY;
	if (edit >= 0) {
		u8 *name = best[edit].name;
		if (c0 != c) {
			c0 = c;
			timer = 10;
		}
		else if (timer) {
			c = 0;
			timer--;
		}
		else timer = 2;
		if (c & KEY_LEFT && --name[pos] < '@') name[pos] = 'Z';
		if (c & KEY_RIGHT && ++name[pos] > 'Z') name[pos] = '@';
		if (c & KEY_B && pos > 0) name[pos--] = '@';
		if (c & KEY_A)
			if (pos < 7) name[++pos] = 'A';
			else {
				playStopAll();
				return 0;
			}
	}
	else if (c & KEY_A) return 0;
	return 1;
}

void nameMain(void) {
	u8 t = 0;
	vramSingle();
	cls();
	chrInit();
	scorePrint();
	chrFlush();
	nameInit();
	while (nameUpdate()) {
		if (isDemo() && ++t >= 100) return;
		scoreUpdate();
		chrUpdate();
		waitVSync(3);
	}
	demoSet(0);
}
