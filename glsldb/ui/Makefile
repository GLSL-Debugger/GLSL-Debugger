include ../../common.mk

CXXFLAGS += -I$(QTDIR)/include -I../glsldb
LDFLAGS += -L$(QTDIR)/lib -lQtCore -lQtGui

all: mainWindow.ui.h openProgramDialog.ui.h editCallDialog.ui.h \
     jumpToDialog.ui.h selectionDialog.ui.h loopDialog.ui.h dbgShaderView.ui.h \
     watchScalar.ui.h watchVector.ui.h watchTable.ui.h watchGeoDataTree.ui.h \
     fragmentTestDialog.ui.h compilerErrorDialog.ui.h compilerErrorDetails.ui.h \
	 attachToProcessDialog.ui.h glTraceSettingsDialog.ui.h aboutBox.ui.h

%.ui.h: %.ui
	$(UIC) $< -o $@

clean:
	rm -f *.ui.h
	rm -f *.o *~

depend:
	echo ""

.PHONY: clean depend

# DO NOT DELETE
