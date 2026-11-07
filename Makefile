MPU = Z80

BG = 1
SND = 0
FULL = 0

# set default

MPU := $(shell echo $(MPU) | tr Z z)
AS = as$(MPU)
CC = fcc
LD = ld$(MPU)
NM = nm$(MPU)
MKPAT = ./mkpat.pl
MKBG = ./mkbg.pl

# setting the compiler to use

ifeq ($(MPU),6800)
CC = chibicc
endif

ifeq ($(MPU),6809)
CC = cmoc
endif

ifeq ($(MPU),6502)
CC = cc65
endif

ifeq ($(MPU),z80)
CC = sdcc
endif

# group

ifeq ($(MPU),6800)
IS68 = 1
endif
ifeq ($(MPU),6809)
IS68 = 1
endif

ifeq ($(MPU),8080)
IS80 = 1
endif
ifeq ($(MPU),z80)
IS80 = 1
endif

# settings for each compiler

ifeq ($(CC),chibicc)
CFLAGS = -O -S
OBJS = crt0-chibi.o
LIBS = /opt/chibicc/lib/libc.a
LDFLAGS = -b -C0x100 -m a.map
endif

ifeq ($(CC),cmoc)
AS = lwasm
LD = lwlink
ASFLAGS = -fobj
CFLAGS = -Wno-const --usim -i -c
OBJS = crt0-cmoc.o
LIBS =
LDFLAGS = -ma.map -slinkerscript -L/usr/local/share/cmoc/lib -fraw -lcmoc-crt-usim
NM =
MKPAT = ./mkpat_cmoc.pl
MKBG = ./mkbg_cmoc.pl
endif

ifeq ($(CC),cc65)
OBJS = crt0-cc65.o
AS = ca65
LD = ld65
ASFLAGS = -I /opt/cc65/share/cc65/asminc
CFLAGS = -Cl -Oi
LDFLAGS = -L /opt/cc65/share/cc65/lib -C ld65.cfg -m a.map
LIBS = none.lib
NM =
MKPAT = ./mkpat_cc65.pl
MKBG = ./mkbg_cc65.pl
endif

ifeq ($(CC),sdcc)
AS = sdasz80
ASFLAGS = -lsw
CFLAGS = -c -mz80
LD = sdldz80
LDFLAGS = -nf main.lk
NM =
OBJS = crt0-sdcc.o
MKPAT = ./mkpat_sdcc.pl
MKBG = ./mkbg_sdcc.pl
endif

ifeq ($(CC),fcc)
OBJS = crt0-$(MPU).o
LIBS = /opt/fcc/lib/$(MPU)/lib$(MPU).a
LDFLAGS = -b -m a.map
ifeq ($(MPU),6502)
CFLAGS = -m$(MPU) -Os -S
LDFLAGS += -C0x200
else
CFLAGS = -m$(MPU) -O -S
endif
ifneq ($(IS68),)
LDFLAGS += -C0x100
endif
ifneq ($(IS80),)
LDFLAGS += -C0x80
endif
ifeq ($(MPU),8070)
CFLAGS += -DINS8070
LDFLAGS += -C1 -Z0xff00
endif
endif

OBJS += main.o base.o mathi.o game.o chr.o emitter.o es.o sprite.o sp.o pattern.o bg.o

# settings for options

ifneq ($(BG),0)
CFLAGS += -DBG_ENABLE
OBJS += bgdata.o
endif

ifneq ($(SND),0)
CFLAGS += -DSND_ENABLE
OBJS += music.o play.o tone.o
endif
ifneq ($(FULL),0)
CFLAGS += -DFULL
OBJS += title.o name.o
endif

#

CLEAN_ASM := $(shell echo $(OBJS) | sed -e 's/crt0-[0-9a-z]*\.o //' | sed -e 's/.o/.asm/g')

ifeq ($(CC),sdcc)
OBJS := $(shell echo $(OBJS) | sed -e 's/.o/.rel/g')
endif

.SUFFIXES: .asm .rel


all: pattern.h music.h $(OBJS)
ifeq ($(CC),sdcc)
	$(LD) $(LDFLAGS)
	sdobjcopy -I ihex -O binary a.ihx a.out
else
	$(LD) $(LDFLAGS) -o a.out $(OBJS) $(LIBS)
endif
ifeq ($(CC),cmoc)
	mv a.out a.tmp
	dd if=/dev/zero of=a.out bs=256 count=1
	cat a.tmp >> a.out
endif
ifeq ($(CC),cc65)
	mv a.out a.tmp
	dd if=/dev/zero of=a.out bs=512 count=1
	cat a.tmp >> a.out
endif
ifneq ($(IS68),)
	f9dasm -$(MPU) a.out > a.lst
endif
ifneq ($(IS80),)
	z80dasm -l -t -g 0 a.out > a.lst
endif
ifeq ($(MPU),6502)
	dcc6502 -c -d -o 512 -s 512 a.out > a.lst
endif

ifneq ($(NM),)
	./mkadr.pl $(NM) > a.adr
endif
	./total.pl

.asm.o:
	$(AS) $(ASFLAGS) -l$(@:.o=.lst) -o $@ $<

.s.o:
	$(AS) $(ASFLAGS) -l$(@:.o=.lst) -o $@ $<

.asm.rel:
	$(AS) $(ASFLAGS) -o $@ $<

.s.rel:
	$(AS) $(ASFLAGS) -o $@ $<

bgdata.s: $(MKBG) map.bmp
	$(MKBG) map.bmp

es.c: essrc mkes.pl
	./mkes.pl $< > $@

music.h music.s: mkmusic
	./mkmusic

mkmusic: msrc.cpp mkmusic.h mml.c tone.c
	c++ -std=c++11 -Wno-deprecated -o $@ $< mml.c tone.c

tone.c: mktone.pl
	./mktone.pl >$@

pattern.h pattern.s: $(MKPAT) sprite.tim
	$(MKPAT) sprite.tim
	
.c.o:
	$(CC) $(CFLAGS) $<
ifneq ($(CC),cmoc)
	$(AS) $(ASFLAGS) -l$(<:.c=.lst) -o $@ $(<:.c=.s)
endif

.c.rel:
	$(CC) $(CFLAGS) $<


clean:
	rm -f bgconf.h es.c mkmusic music.h pattern.* tone.c
	rm -f *.{lst,o,rel,s,sym} a.{adr,ihx,map,noi,out,tmp}
	rm -f $(CLEAN_ASM)
