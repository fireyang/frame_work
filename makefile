
#SUBDIRS := $(shell ls -d */)

SUBDIRS := framework public third_party  gate_server 


.PHONY:all $(SUBDIRS)

all : $(SUBDIRS)
#	@for SUBDIR in $(SUBDIRS) ; do make -C $$SUBDIR all ; done


$(SUBDIRS):
	$(MAKE) -C $@
clean :
	@for SUBDIR in $(SUBDIRS) ; do make -C $$SUBDIR clean ; done
release: clean
	@for SUBDIR in $(SUBDIRS) ; do make -C $$SUBDIR release=1 ; done


