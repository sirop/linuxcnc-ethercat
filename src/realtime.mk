include ../config.mk
include Kbuild

cc-option = $(shell if $(CC) $(CFLAGS) $(1) -S -o /dev/null -xc /dev/null \
             > /dev/null 2>&1; then echo "$(1)"; else echo "$(2)"; fi ;)

.PHONY: all install

ifeq ($(BUILDSYS),kbuild)

module = $(patsubst %.o,%.ko,$(obj-m))

ifeq (,$(findstring -Wframe-larger-than=,$(EXTRA_CFLAGS)))
  EXTRA_CFLAGS += $(call cc-option,-Wframe-larger-than=2560)
endif

$(module):
	$(MAKE) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" KBUILD_EXTRA_SYMBOLS="$(RTLIBDIR)/Module.symvers $(RTAIDIR)/modules/ethercat/Module.symvers" -C $(KERNELDIR) SUBDIRS=`pwd` CC=$(CC) V=0 modules

else

module = $(patsubst %.o,%.so,$(obj-m))

EXTRA_CFLAGS +=  -fPIC -g -funwind-tables

$(module): $(lcec-objs)
	@echo Linking $@
	ld -d -r -o $*.tmp $^
	objcopy -j .rtapi_export -O binary $*.tmp $*.exported
	(echo '{ global : '; tr -s '\0' <$*.exported | xargs -r0 printf '%s;\n' | grep .; echo 'local : * ; };') > $*.ver
	$(CC) -shared -Bsymbolic -Wl,-rpath,$(LIBDIR) -L$(LIBDIR)  -Wl,--version-script,$*.ver -o $@ $^ -llinuxcnchal -lethercat_rtdm -lrt
    #-Wl,--no-as-needed
	ld -d -r -o $*.tmp $^
	@echo Linking $@ end
%.o: %.c
	$(CC) -o $@ $(EXTRA_CFLAGS) -Os -c $<

endif

all: $(module)

install: $(module)
	mkdir -p $(DESTDIR)$(RTLIBDIR)
	cp $(module) $(DESTDIR)$(RTLIBDIR)/

