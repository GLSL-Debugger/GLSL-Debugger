# - FindQtDll
# This module finds Qt dll files
#
#  DLL_FILES_QT - qt dll files required in distribution

set(DLL_FILES_QT "")
foreach(module QT3SUPPORT QTOPENGL QTASSISTANT QTDESIGNER QTMOTIF QTNSPLUGIN
               QAXSERVER QAXCONTAINER QTDECLARATIVE QTSCRIPT QTSVG QTUITOOLS QTHELP
               QTWEBKIT PHONON QTSCRIPTTOOLS QTMULTIMEDIA QTGUI QTTEST QTDBUS QTXML QTSQL
               QTXMLPATTERNS QTNETWORK QTCORE)
	if (QT_USE_${module} OR QT_USE_${module}_DEPENDS)
		if(CMAKE_BUILD_TYPE MATCHES RELEASE)
			set(module "${QT_${module}_LIBRARY_RELEASE}")
		else()
			set(module "${QT_${module}_LIBRARY_DEBUG}")
		endif()
		get_filename_component(filename "${module}" NAME_WE)
        list(APPEND DLL_FILES_QT "${QT_BINARY_DIR}/${filename}.dll")
	endif()
endforeach()
