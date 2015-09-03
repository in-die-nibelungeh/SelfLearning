
SUBDIRS := Asp \
	Buffer \
	FFT \
	FileIo \
	WaveGen \

$(MAKECMDGOALS) _root: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: _root $(SUBDIRS)
