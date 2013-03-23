include common.mk

SUBDIRS = GLSLCompiler glsldb
RELEASE_BDIR = glsldevil/glsldevil-$(shell sed -n 's/Version\W\+\(.\+\)\W*/\1/p' glsldb/doc/readme.txt)
RELEASE_DIR = $(RELEASE_BDIR)/release$(POSTFIX)
RELEASE_LIBDIR = $(RELEASE_DIR)/libs
RELEASE_PLGDIR = $(RELEASE_LIBDIR)/plugins$(POSTFIX)

all:
	for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir $@ ; done

clean:
	for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir $@ ; done

depend:
	for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir $@ ; done
	
cleandepend:
	for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir $@ ; done

release: all
	@-[ -e $(RELEASE_DIR) ] && echo "$(RELEASE_DIR) already exists!"
	@[ ! -e $(RELEASE_DIR) ]
	mkdir -p $(RELEASE_PLGDIR)
	cp glsldb/glsldb $(RELEASE_DIR)/
	cp glsldb/DebugLib/{libdlsym-$(POSTFIX).so,libglsldebug-$(POSTFIX).so} $(RELEASE_LIBDIR)/
	cp glsldb/DebugFunctions/glEnd.so $(RELEASE_PLGDIR)/
	cp glsldb/doc/readme.txt $(RELEASE_DIR)/
	html2text -nobs -style pretty glsldb/doc/license.txt > $(RELEASE_DIR)/license.txt
	echo "<br><hr>" | html2text -nobs -style pretty >> $(RELEASE_DIR)/license.txt
	html2text -nobs -style pretty glsldb/doc/credits.txt >> $(RELEASE_DIR)/license.txt
	strip -s $(RELEASE_DIR)/glsldb
	strip --strip-unneeded $(RELEASE_LIBDIR)/{libdlsym-$(POSTFIX).so,libglsldebug-$(POSTFIX).so}
	strip --strip-unneeded $(RELEASE_PLGDIR)/glEnd.so
	chmod 644 $(RELEASE_LIBDIR)/*.so  $(RELEASE_PLGDIR)/*.so $(RELEASE_DIR)/*.txt
	chmod 755 $(RELEASE_DIR)/glsldb

.PHONY: clean depend cleandepend release
