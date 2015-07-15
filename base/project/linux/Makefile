
LIBFILES=common/libcommon.a os/libos.a
CLEAN_LIB := $(LIBFILES:%.a=clean_%.a)

all: $(LIBFILES)
%.a:
	$(if $(findstring clean_,$@),$(MAKE) -C $(subst clean_,,$(@D)) clean,$(MAKE) -C $(@D))
clean: $(CLEAN_LIB)
	rm -rf ../../lib/linux/debug/*.a
	rm -rf ../../lib/linux/release/*.a
