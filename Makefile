lc = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))

UNAME_MACHINE=$(call lc,$(shell uname -m))
UNAME_SYSTEM=$(call lc,$(shell uname -s))

TARGET=${UNAME_SYSTEM}-${UNAME_MACHINE}

BIN?=playground-lvgl

DEPS:=
DEPS+=$(wildcard src/*.c)
DEPS+=$(wildcard src/*/*.c)
DEPS+=$(wildcard src/*/assets/*.json)
DEPS+=$(wildcard src/*/components/*.xml)
DEPS+=$(wildcard src/*/components/*/*.xml)
# DEPS+=$(wildcard src/*/screens/*.xml)
DEPS+=$(wildcard target/${TARGET}/src/*.c)
DEPS+=target/${TARGET}/src/lv_conf.h

.PHONY: default
default: build/output/${BIN}

build: ${DEPS}
	mkdir -p build
	cp --verbose --force --recursive --preserve=all src build/
	cp --verbose --force --recursive --preserve=all lib build/
	cp --verbose --force --recursive --preserve=all target/${TARGET}/* build/

build/output/${BIN}: build
	${MAKE} --directory build/ -j

.PHONY: start
start: build/output/${BIN}
	build/output/${BIN}

.PHONY: clean
clean:
	rm -rf build
