SUBDIRS = GenericCodeGen DebugCodeGen MachineIndependent OSDependent/Linux

all:
	for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir $@ ; done

clean:
	for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir $@ ; done

depend:
	for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir $@ ; done
	
cleandepend:
	for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir $@ ; done

