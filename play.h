#include "types.h"

#define PF_LOOP		1

#ifdef SND_ENABLE
void playInit(void);
void playStart(u8 index, u16 ofs, u8 flags);
void playStop(u8 index);
void playStopAll(void);
void playMute(u8 f);
u16 playing(u8 index);
#else
#define playInit()
#define playStart(index, ofs, flags)
#define playStop(index)
#define playStopAll()
#define playMute(f)
#define playing(index) 0
#endif
