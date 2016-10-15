ifneq ($(PLATFORM), PC)
$(info *** Defaulting to GCW version. Specify PLATFORM=PC for PCLINUX test build. ***)
SDL_CONFIG=/opt/gcw0-toolchain/usr/mipsel-gcw0-linux-uclibc/sysroot/usr/bin/sdl-config
TOOLCHAIN=/opt/gcw0-toolchain/usr/bin/mipsel-linux-
LIB_INCLUDE=/opt/gcw0-toolchain/usr/mipsel-gcw0-linux-uclibc/sysroot/usr/lib
else
$(info *** Making PCLINUX test build. Unspecify PLATFORM=PC for GCW build. ***)
SDL_CONFIG=/usr/bin/sdl-config
TOOLCHAIN=
LIB_INCLUDE=/usr/lib
endif

OPK_DIR=opk_build
CC=$(TOOLCHAIN)gcc
CXX=$(TOOLCHAIN)g++
STRIP=$(TOOLCHAIN)strip
EXE = aaaa
OPK = $(EXE).opk
RM = rm -f

LDFLAGS +=-lSDL_gfx -lSDL_image -lSDL_mixer $(shell $(SDL_CONFIG) --libs) -lGLESv1_CM -lEGL
CFLAGS += -O2 $(shell $(SDL_CONFIG) --cflags) -DGCW

ifeq ($(PLATFORM), PC)
LDFLAGS += -lX11 -lshake
CFLAGS += -DPC
else
LDFLAGS += ./opk_build/libshake.so.1
CFLAGS += -DNOHAPTIC
endif


REMOTE_USER=root
REMOTE_IP=192.168.2.104
REMOTE_PATH=/media/GCW_EXT/apps

SRCS=$(shell echo *.c)
OBJS=$(SRCS:%.c=%.o)

ALL : $(EXE)

.c.o:
	$(CC) $(CFLAGS) $(CXXFLAGS) -c $*.c -o $*.o 
.cpp.o:
	$(CXX) $(CFLAGS) $(CXXFLAGS) -c $*.cpp -o $*.o 

$(EXE) : $(OBJS)
	$(CXX) $(OBJS) -o $(EXE) $(LDFLAGS)
	$(STRIP) $(EXE)
	cp $(EXE) $(OPK_DIR)

opk : $(EXE)
	mksquashfs opk_build $(EXE).opk -all-root -noappend -no-exports -no-xattrs

upload : opk
	scp ./$(OPK) $(REMOTE_USER)@$(REMOTE_IP):$(REMOTE_PATH)/$(OPK)

clean :
	$(RM) $(OBJS) $(EXE) $(OPK_DIR)/$(EXE) $(OPK)

.PHONY:opk clean install uninstall ALL
