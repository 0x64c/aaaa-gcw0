OPK_DIR=opk_build
SDL_CONFIG=/opt/gcw0-toolchain/usr/mipsel-gcw0-linux-uclibc/sysroot/usr/bin/sdl-config
TOOLCHAIN=/opt/gcw0-toolchain/usr/bin/mipsel-linux-

LIB_INCLUDE=/opt/gcw0-toolchain/usr/mipsel-gcw0-linux-uclibc/sysroot/usr/lib
CC=$(TOOLCHAIN)gcc
CXX=$(TOOLCHAIN)g++
STRIP=$(TOOLCHAIN)strip

EXE = aaaa
OPK = $(EXE).opk
RM = rm -f


# Add SDL dependency
LDFLAGS +=-lSDL_gfx -lSDL_image -lSDL_mixer $(shell $(SDL_CONFIG) --libs) -lGLESv1_CM -lEGL -lshake
CFLAGS +=$(shell $(SDL_CONFIG) --cflags) -g -DGCW

#LDFLAGS +=-lSDL_gfx -lSDL_image -lSDL_mixer $(shell sdl-config --libs) -lGL
#CFLAGS +=$(shell sdl-config --cflags) -DPC32

# Define targets
SRCS=$(shell echo *.c)
OBJS=$(SRCS:%.c=%.o)

ALL : $(EXE)

.c.o:
	$(CC) $(CFLAGS) $(CXXFLAGS) -c $*.c -o $*.o 
.cpp.o:
	$(CXX) $(CFLAGS) $(CXXFLAGS) -c $*.cpp -o $*.o 

$(EXE) : $(OBJS)
	$(CXX) $(OBJS) -o $(EXE) $(LDFLAGS)
#	$(STRIP) $(EXE)

opk : $(EXE)
	cp $(EXE) $(OPK_DIR)
	mksquashfs opk_build $(EXE).opk -all-root -noappend -no-exports -no-xattrs

#install : $(EXE)
#	$(INSTALL_PROG) $(EXE) $(PREFIX)/bin

#uninstall :
#	$(RM) $(PREFIX)/bin/$(EXE)

clean :
	$(RM) $(OBJS) $(EXE) $(OPK_DIR)/$(EXE) $(OPK)

.PHONY:opk clean install uninstall ALL
