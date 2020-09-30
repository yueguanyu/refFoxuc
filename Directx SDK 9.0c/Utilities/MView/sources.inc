!include d3dx.mk

SYNCHRONIZE_DRAIN=1
LANGUAGE_NEUTRAL=0

TARGETNAME=MView

MAJORCOMP=
MINORCOMP=

TARGETPATH=obj
TARGETTYPE=PROGRAM
TARGETLIBS=$(BASEDIR)\Public\sdk\lib\*\d3d9.lib \
           $(BASEDIR)\Public\sdk\lib\*\winmm.lib \
           $(BASEDIR)\Public\sdk\lib\*\comctl32.lib \
           $(BASEDIR)\Public\sdk\lib\*\shell32.lib \
           $(BASEDIR)\Public\sdk\lib\*\mfc42d.lib \
           $(BASEDIR)\Public\sdk\lib\*\comdlg32.lib \
           $(BASEDIR)\Public\sdk\lib\*\d3dxof.lib \
           $(BASEDIR)\Public\sdk\lib\*\version.lib \
           $(BASEDIR)\Public\sdk\lib\*\dxerr8.lib \
           ..\gxu\$(_OBJ_DIR)\*\gxu.lib \
           $(BASEDIR)\Public\sdk\lib\*\uuid.lib \
           $(BASEDIR)\Public\sdk\lib\*\uuid.lib 

!if $(FREEBUILD)
TARGETLIBS=$(TARGETLIBS) $(BASEDIR)\Public\sdk\lib\*\d3dx9.lib 
!else 
!if defined(D3DXDEBUG)
TARGETLIBS=$(TARGETLIBS) $(BASEDIR)\Public\sdk\lib\*\d3dx9d.lib 
!else
TARGETLIBS=$(TARGETLIBS) $(BASEDIR)\Public\sdk\lib\*\d3dx9dt.lib 
!endif
!endif

INCLUDES=\
         ..\gxu; \
	 ..;\
         $(DXROOT)\inc; \
         $(SDK_INC_PATH); \
         $(BASEDIR)\public\sdk\inc; \
         $(BASEDIR)\public\sdk\inc\mfc42;

SOURCES=..\mview.rc \
        ..\mview.cpp \
        ..\mdraw.cpp \
        ..\meshop.cpp \
        ..\mode.cpp \
        ..\wndproc.cpp \
        ..\mload.cpp \
        ..\dlgproc.cpp \
        ..\texture.cpp \
        ..\mshape.cpp \
        ..\memdbg.cpp \
        ..\d3dxapp.cpp \
	..\util.cpp \
        ..\treeview.cpp

PRECOMPILED_INCLUDE=..\mviewpch.h
PRECOMPILED_CXX=1

C_DEFINES= -DWIN32 -D_WIN32_WINNT=0x0500 -D_WIN32_IE=0x0501

USE_MSVCRT=1
UMENTRY=winmain
UMTYPE=windows
UMAPPL=
UMLIBS=
CHICAGO_PRODUCT=1
386_STDCALL=0
WIN32_IE_VERSION=0x0000
