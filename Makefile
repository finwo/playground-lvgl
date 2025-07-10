lc = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))

UNAME_MACHINE=$(call lc,$(shell uname -m))
UNAME_SYSTEM=$(call lc,$(shell uname -s))

TARGET=${UNAME_SYSTEM}-${UNAME_MACHINE}

BIN?=playground-lvgl

DEPS:=
DEPS+=$(wildcard src/*.c)
DEPS+=$(wildcard src/*/*.c)
DEPS+=$(wildcard src/*/components/*.xml)
DEPS+=$(wildcard src/*/components/*/*.xml)
DEPS+=$(wildcard target/${TARGET}/src/*.c)
DEPS+=target/${TARGET}/src/lv_conf.h

.PHONY: default
default: ${BIN}

build: ${DEPS}
	mkdir -p build
	rsync -a src build/
	rsync -a lib build/
	rsync -a target/${TARGET}/* build/

${BIN}: build
	${MAKE} --directory build/ -j
	mv build/playground-lvgl .

.PHONY: clean
clean:
	rm -rf build
