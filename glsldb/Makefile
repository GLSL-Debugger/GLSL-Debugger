include ../common.mk

CXXFLAGS += -I$(QTDIR)/include -I. -I utils -I ../GLSLCompiler/glslang/Public \
            -DLIBPOSTFIX=\"$(POSTFIX)\"

LDFLAGS += -L$(QTDIR)/$(ARCHLIB) -lQtCore -lQtGui -lQtOpenGL -lGLU -lGL -lX11 -lpthread

LIBS = ../GLSLCompiler/glslang/MachineIndependent/lib/libglslang.a \
       ./glenumerants/libglenumerants.a \
       ./DebugLib/libfunctionList.a \
       ./utils/libutils.a

SUBDIRS = ui utils glenumerants DebugLib DebugFunctions 

all: libDir subdirs glsldb

libDir:
	mkdir -p ./libs

subdirs:
	for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir ; done

%.moc.cpp: %.qt.h
	$(MOC) $< -o $@

%.res.cpp: %.qrc
	$(RCC) $< -o $@

OBJECTS = glsldb.o pixelBox.moc.o pixelBox.o functionCall.o glCallStatistics.o \
          glslDevil.res.o progControl.o glslSyntaxHighlighter.moc.o \
          glslSyntaxHighlighter.o shVarModel.moc.o shVarModel.o \
          openProgramDialog.moc.o openProgramDialog.o editCallDialog.moc.o \
          editCallDialog.o jumpToDialog.moc.o jumpToDialog.o \
          textPercentDelegate.o textPercentDelegate.moc.o watchView.moc.o \
          curveView.moc.o curveView.o vertexBox.o vertexBox.moc.o \
          watchView.o imageView.o imageView.moc.o \
          watchVector.moc.o watchVector.o mainWindow.moc.o mainWindow.o selectionDialog.moc.o \
          selectionDialog.o loopData.moc.o loopData.o loopDialog.moc.o \
          loopDialog.o dbgShaderView.moc.o dbgShaderView.o errorCodes.o \
          vertexTableModel.o vertexTableModel.moc.o watchTable.o watchTable.moc.o \
          geoShaderDataModel.o geoShaderDataModel.moc.o geoShaderTreeItems.o \
          watchGeoDataTree.o watchGeoDataTree.moc.o glScatter.moc.o glScatter.o \
		  fragmentTestDialog.o fragmentTestDialog.moc.o \
          mappings.o mmath.o compilerErrorDialog.o compilerErrorDialog.moc.o \
          compilerErrorDetails.o compilerErrorDetails.moc.o attachToProcessDialog.o \
          attachToProcessDialog.moc.o attachToProcess.o attachToProcess.moc.o \
		  glTraceListModel.o glTraceSettingsDialog.moc.o glTraceSettingsDialog.o \
		  glTraceFilterModel.o aboutBox.o aboutBox.moc.o

glsldb: $(OBJECTS) $(LIBS)
	$(CXX) -o $@ $(OBJECTS) $(LIBS) $(LDFLAGS)

# todo: add icons and cursors
glslDevil.res.cpp: doc/readme.txt doc/license.txt doc/credits.txt

guiclean:
	rm -f *.o *~ glsldb
	makedepend
	$(MAKE) -C ui clean
	
clean: cleandepend
	rm -f *.o *~ glsldb
	rm -rf ./libs
	for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir $@ ; done

depend:
	makedepend  -- $(CFLAGS) -- *.[ch]*
	for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir $@ ; done

cleandepend:
	makedepend
	for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir $@ ; done

tags:
	ctags --recurse=yes *.[ch] *.cpp $(SUBDIRS)

.PHONY: clean cleandepend depend subdirs tags guiclean

# DO NOT DELETE
