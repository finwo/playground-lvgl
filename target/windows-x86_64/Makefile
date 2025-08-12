lc = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))

LIBS:=
SRC:=
BIN?=playground-lvgl.exe
INCLUDES:=

UNAME_MACHINE=$(call lc,$(shell uname -m))
UNAME_SYSTEM=$(call lc,$(shell uname -s))

CC:=clang
CPP:=clang

SRC+=$(wildcard src/*.c)
SRC+=$(wildcard src/*/*.c)

override CFLAGS?=-Wall -O2
override CFLAGS+=-I src -DDLV_CONF_INCLUDE_SIMPLE
override LDFLAGS?=
override CPPFLAGS?=

# override LDFLAGS+=$(shell pkg-config --libs sdl2)

# override CFLAGS+=-D WEBVIEW_STATIC
# override CFLAGS+=-D WINTERM

ifeq ($(OS),Windows_NT)
	override CFLAGS += -D WIN32 -D _WIN32
	# override CPPFLAGS+=-lstdc++
	ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
		override CFLAGS += -D AMD64 -D _WIN64
	else
		ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
			override CFLAGS += -D AMD64 -D _WIN64
		endif
		ifeq ($(PROCESSOR_ARCHITECTURE),x86)
			# CFLAGS += -D IA32
		endif
	endif
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		# CFLAGS += -D LINUX
		override CPPFLAGS+=-lstdc++
		override CFLAGS+=$(shell pkg-config --cflags glib-2.0)
		override LDFLAGS+=$(shell pkg-config --libs glib-2.0)
		override CFLAGS+=-D _GNU_SOURCE
	endif
	ifeq ($(UNAME_S),Darwin)
		# CFLAGS += -D OSX
		override CPPFLAGS+=-std=c++14
		override CFLAGS+=-D _BSD_SOURCE
	endif
	UNAME_P := $(shell uname -p)
	ifeq ($(UNAME_P),x86_64)
		# CFLAGS += -D AMD64
	endif
	ifneq ($(filter %86,$(UNAME_P)),)
		# CFLAGS += -D IA32
	endif
	ifneq ($(filter arm%,$(UNAME_P)),)
		# CFLAGS += -D ARM
	endif
	# TODO: flags for riscv
endif

include lib/.dep/config.mk

OBJ:=$(SRC:.c=.o)
OBJ:=$(OBJ:.cc=.o)

LOG_USE_COLOR?=0
ifeq ($(LOG_USE_COLOR),1)
  override CFLAGS+=-DLOG_USE_COLOR
endif

override CFLAGS+=$(INCLUDES)
override CPPFLAGS+=$(INCLUDES)
override CPPFLAGS+=$(CFLAGS)

override CFLAGS+=-DLV_USE_LABEL

includeFiles:=
includeFiles+=src/AppModule/assets/global.json
includeFiles+=src/AppModule/assets/offline-resources-1x.png
includeFiles+=src/AppModule/assets/offline-resources-2x.png
includeFiles+=src/AppModule/assets/public-pixel.ttf
# includeFiles+=$(wildcard src/*/screens/*.xml)

.PHONY: default
default: output/$(BIN) output/assets

$(OBJ):

lib/.dep/config.mk:
	bash -c "cp ../package.ini . && dep i"

.cc.o:
	$(CPP) $< $(CPPFLAGS) -c -o $@

.c.o:
	$(CC) $< $(CFLAGS) -c -o $@

output/${BIN}: ${OBJ}
	$(CPP) $(OBJ) $(CPPFLAGS) $(LDFLAGS) -s -o $@

output/assets: ${includeFiles}
	for asset in ${includeFiles}; do \
		mkdir -p output/$$(dirname $$(echo $$asset | cut -d "/" -f 3-)) ; \
		cp $$asset output/$$(dirname $$(echo $$asset | cut -d "/" -f 3-)) ; \
	done

.PHONY: clean
clean:
	rm -rf ${OBJ}
	rm -rf ${includeHeaders}
