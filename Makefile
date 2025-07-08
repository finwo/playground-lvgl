lc = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))

BIN?=playground-lvgl

UNAME_MACHINE=$(call lc,$(shell uname -m))
UNAME_SYSTEM=$(call lc,$(shell uname -s))

BIN2C:="component/tool-bin2c/bin2c-${UNAME_SYSTEM}-${UNAME_MACHINE}"

.PHONY: default
default: $(BIN)

${BIN2C}:
	${MAKE} --directory "$(shell dirname "${BIN2C}")"

component/app/lib/.dep/config.mk:
	bash -c "cd component/app && dep i"

$(BIN): ${BIN2C} component/app/lib/.dep/config.mk
	${MAKE} --directory "component/app" BIN2C=${BIN2C} -j
	mv --force "component/app/$@" "$@"

.PHONY: clean
clean:
	rm -rf "${BIN2C}"
	${MAKE} --directory "$(shell dirname "${BIN2C}")" clean
	${MAKE} --directory "component/app" clean

.PHONY: sterile
sterile: clean
	rm -rf "component/app/lib"
