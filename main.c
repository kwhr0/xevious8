#include "game.h"
#include "base.h"
#include "play.h"
#include "title.h"
#include "name.h"

int main(void) {
	u8 state = 0;
	ei();
#ifndef FULL
	scoreReset();
	gameInit();
	gameMain();
#else
	do {
		switch (state) {
		case 0:
			demoSet(1);
			playMute(1);
			titleMain(0);
			state = isDemo() ? 1 : 4;
			break;
		case 1:
			gameInit();
			gameMain();
			state = isDemo() ? 2 : 4;
			break;
		case 2:
			nameEntry(0);
			nameMain();
			state = isDemo() ? 3 : 4;
			break;
		case 3:
			gameInit();
			gameMain();
			state = isDemo() ? 0 : 4;
			break;
		case 4:
			playMute(0);
			titleMain(1);
			scoreReset();
			gameInit();
			state = 5;
			break;
		case 5:
			gameMain();
			if (reserveDec()) state = nameEntry(scoreGet()) ? 6 : 9;
			break;
		case 6:
			nameMain();
			state = 9;
			break;
		default:
			gameoverMain();
			state = 0;
			break;
		}
	} while (1);
#endif
	return 0;
}
