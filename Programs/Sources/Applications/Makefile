
SUBDIRS_EXCLUDE := Makefile \
    SpeakerCorrection \
    Fir2Iir \
    MakeFir \
    FourieTransfer \
    PcmTool \
    WaveConverter \
    WaveGenerator \

SUBDIRS := $(filter-out $(SUBDIRS_EXCLUDE), $(wildcard *))

$(MAKECMDGOALS) _root: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: _root $(SUBDIRS)
