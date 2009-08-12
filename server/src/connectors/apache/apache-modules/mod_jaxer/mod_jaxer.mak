# Microsoft Developer Studio Generated NMAKE File, Based on mod_jaxer.dsp
!IF "$(CFG)" == ""
CFG=mod_jaxer - Win32 Release
!MESSAGE No configuration specified. Defaulting to mod_jaxer - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "mod_jaxer - Win32 Release" && "$(CFG)" != "mod_jaxer - Win32 Debug" && "$(CFG)" != "mod_jaxer - Win32 Debug20" && "$(CFG)" != "mod_jaxer - Win32 Release20"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mod_jaxer.mak" CFG="mod_jaxer - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mod_jaxer - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mod_jaxer - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mod_jaxer - Win32 Release20" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mod_jaxer - Win32 Debug20" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "mod_jaxer - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
AP_INC_DIR=..\..\apache\apache22-include
# End Custom Macros

ALL : "$(OUTDIR)\mod_jaxer.so"


CLEAN :
	-@erase "$(INTDIR)\mod_jaxer_conf.obj"
	-@erase "$(INTDIR)\mod_jaxer_connection.obj"
	-@erase "$(INTDIR)\mod_jaxer_proc.obj"
	-@erase "$(INTDIR)\mod_jaxer_filter.obj"
	-@erase "$(INTDIR)\mod_jaxer_message.obj"
	-@erase "$(INTDIR)\mod_jaxer.obj"
	-@erase "$(INTDIR)\mod_jaxer.res"
	-@erase "$(INTDIR)\mod_jaxer_src.idb"
	-@erase "$(INTDIR)\mod_jaxer_src.pdb"
	-@erase "$(OUTDIR)\mod_jaxer.exp"
	-@erase "$(OUTDIR)\mod_jaxer.lib"
	-@erase "$(OUTDIR)\mod_jaxer.pdb"
	-@erase "$(OUTDIR)\mod_jaxer.so"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /Zi /O2 /I "." /I "../../apache/apache22-include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\mod_jaxer_src" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mod_jaxer.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mod_jaxer.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib ws2_32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\mod_jaxer.pdb" /debug /machine:I386 /out:"$(OUTDIR)\mod_jaxer.so" /implib:"$(OUTDIR)\mod_jaxer.lib" /base:@BaseAddr.ref,mod_jaxer.so /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\mod_jaxer_connection.obj" \
	"$(INTDIR)\mod_jaxer_conf.obj" \
	"$(INTDIR)\mod_jaxer_filter.obj" \
	"$(INTDIR)\mod_jaxer_proc.obj" \
	"$(INTDIR)\mod_jaxer_message.obj" \
	"$(INTDIR)\mod_jaxer.obj" \
	"$(INTDIR)\mod_jaxer.res" \
	"..\..\apache\apache22-lib\libapr-1.lib" \
	"..\..\apache\apache22-lib\libaprutil-1.lib" \
	"..\..\apache\apache22-lib\libhttpd.lib" 

"$(OUTDIR)\mod_jaxer.so" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mod_jaxer - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
AP_INC_DIR=..\..\apache\apache22-include

ALL : "mod_jaxer.so"


CLEAN :
	-@erase "$(INTDIR)\mod_jaxer_connection.obj"
	-@erase "$(INTDIR)\mod_jaxer_filter.obj"
	-@erase "$(INTDIR)\mod_jaxer_conf.obj"
	-@erase "$(INTDIR)\mod_jaxer_proc.obj"
	-@erase "$(INTDIR)\mod_jaxer_message.obj"
	-@erase "$(INTDIR)\mod_jaxer.obj"
	-@erase "$(INTDIR)\mod_jaxer.res"
	-@erase "$(INTDIR)\mod_jaxer_src.idb"
	-@erase "$(INTDIR)\mod_jaxer_src.pdb"
	-@erase "$(OUTDIR)\mod_jaxer.exp"
	-@erase "$(OUTDIR)\mod_jaxer.lib"
	-@erase "$(OUTDIR)\mod_jaxer.pdb"
	-@erase "mod_jaxer.so"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /I "../../apache/apache22-include" /I "./" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_POSIX_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\mod_jaxer_src" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mod_jaxer.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mod_jaxer.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib ws2_32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\mod_jaxer.pdb" /debug /machine:I386 /out:"$(OUTDIR)\mod_jaxer.so" /implib:"$(OUTDIR)\mod_jaxer.lib" /base:@BaseAddr.ref,mod_jaxer.so /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\mod_jaxer_connection.obj" \
	"$(INTDIR)\mod_jaxer_conf.obj" \
	"$(INTDIR)\mod_jaxer_filter.obj" \
	"$(INTDIR)\mod_jaxer_proc.obj" \
	"$(INTDIR)\mod_jaxer_message.obj" \
	"$(INTDIR)\mod_jaxer.obj" \
	"$(INTDIR)\mod_jaxer.res" \
	"..\..\apache\apache22-lib\libapr-1.lib" \
	"..\..\apache\apache22-lib\libaprutil-1.lib" \
	"..\..\apache\apache22-lib\libhttpd.lib"

"mod_jaxer.so" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Section added to compile with apache 2.0
!ELSEIF  "$(CFG)" == "mod_jaxer - Win32 Release20"

OUTDIR=.\Release20
INTDIR=.\Release20
# Begin Custom Macros
OutDir=.\Release20
AP_INC_DIR=..\..\apache\apache20-include

# End Custom Macros

ALL : "$(OUTDIR)\mod_jaxer20.so"


CLEAN :
	-@erase "$(INTDIR)\mod_jaxer_conf.obj"
	-@erase "$(INTDIR)\mod_jaxer_connection.obj"
	-@erase "$(INTDIR)\mod_jaxer_proc.obj"
	-@erase "$(INTDIR)\mod_jaxer_filter.obj"
	-@erase "$(INTDIR)\mod_jaxer_message.obj"
	-@erase "$(INTDIR)\mod_jaxer.obj"
	-@erase "$(INTDIR)\mod_jaxer.res"
	-@erase "$(INTDIR)\mod_jaxer_src.idb"
	-@erase "$(INTDIR)\mod_jaxer_src.pdb"
	-@erase "$(OUTDIR)\mod_jaxer.exp"
	-@erase "$(OUTDIR)\mod_jaxer20.lib"
	-@erase "$(OUTDIR)\mod_jaxer20.pdb"
	-@erase "$(OUTDIR)\mod_jaxer20.so"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /Zi /O2 /I "." /I "../../apache/apache20-include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_APACHE20" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\mod_jaxer_src" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mod_jaxer.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mod_jaxer.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib ws2_32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\mod_jaxer20.pdb" /debug /machine:I386 /out:"$(OUTDIR)\mod_jaxer20.so" /implib:"$(OUTDIR)\mod_jaxer20.lib" /base:@BaseAddr.ref,mod_jaxer20.so /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\mod_jaxer_connection.obj" \
	"$(INTDIR)\mod_jaxer_conf.obj" \
	"$(INTDIR)\mod_jaxer_filter.obj" \
	"$(INTDIR)\mod_jaxer_proc.obj" \
	"$(INTDIR)\mod_jaxer_message.obj" \
	"$(INTDIR)\mod_jaxer.obj" \
	"$(INTDIR)\mod_jaxer.res" \
	"..\..\apache\apache20-lib\libapr.lib" \
	"..\..\apache\apache20-lib\libaprutil.lib" \
	"..\..\apache\apache20-lib\libhttpd.lib" 

"$(OUTDIR)\mod_jaxer20.so" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mod_jaxer - Win32 Debug20"

OUTDIR=.\Debug20
INTDIR=.\Debug20
AP_INC_DIR=..\..\apache\apache20-include

ALL : "mod_jaxer20.so"


CLEAN :
	-@erase "$(INTDIR)\mod_jaxer_connection.obj"
	-@erase "$(INTDIR)\mod_jaxer_filter.obj"
	-@erase "$(INTDIR)\mod_jaxer_conf.obj"
	-@erase "$(INTDIR)\mod_jaxer_proc.obj"
	-@erase "$(INTDIR)\mod_jaxer_message.obj"
	-@erase "$(INTDIR)\mod_jaxer.obj"
	-@erase "$(INTDIR)\mod_jaxer.res"
	-@erase "$(INTDIR)\mod_jaxer_src.idb"
	-@erase "$(INTDIR)\mod_jaxer_src.pdb"
	-@erase "$(OUTDIR)\mod_jaxer20.exp"
	-@erase "$(OUTDIR)\mod_jaxer20.lib"
	-@erase "$(OUTDIR)\mod_jaxer20.pdb"
	-@erase "mod_jaxer.so"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /I "../../apache/apache20-include" /I "./" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_POSIX_" /D "_APACHE20" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\mod_jaxer_src" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mod_jaxer.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mod_jaxer.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib ws2_32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\mod_jaxer20.pdb" /debug /machine:I386 /out:"$(OUTDIR)\mod_jaxer20.so" /implib:"$(OUTDIR)\mod_jaxer20.lib" /base:@BaseAddr.ref,mod_jaxer20.so /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\mod_jaxer_connection.obj" \
	"$(INTDIR)\mod_jaxer_conf.obj" \
	"$(INTDIR)\mod_jaxer_filter.obj" \
	"$(INTDIR)\mod_jaxer_proc.obj" \
	"$(INTDIR)\mod_jaxer_message.obj" \
	"$(INTDIR)\mod_jaxer.obj" \
	"$(INTDIR)\mod_jaxer.res" \
	"..\..\apache\apache20-lib\libapr.lib" \
	"..\..\apache\apache20-lib\libaprutil.lib" \
	"..\..\apache\apache20-lib\libhttpd.lib"

"mod_jaxer20.so" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

# end of section for apache 2.0

!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("mod_jaxer.dep")
!INCLUDE "mod_jaxer.dep"
!ELSE 
!MESSAGE Warning: cannot find "mod_jaxer.dep"
!ENDIF 
!ENDIF 



!IF "$(CFG)" == "mod_jaxer - Win32 Release" || "$(CFG)" == "mod_jaxer - Win32 Debug" || "$(CFG)" == "mod_jaxer - Win32 Debug20" || "$(CFG)" == "mod_jaxer - Win32 Release20"

SOURCE=.\mod_jaxer_connection.c

"$(INTDIR)\mod_jaxer_connection.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\mod_jaxer_conf.c

"$(INTDIR)\mod_jaxer_conf.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\mod_jaxer_message.c

"$(INTDIR)\mod_jaxer_message.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\mod_jaxer_proc.c

"$(INTDIR)\mod_jaxer_proc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\mod_jaxer_filter.c

"$(INTDIR)\mod_jaxer_filter.obj" : $(SOURCE) "$(INTDIR)"

SOURCE=.\mod_jaxer.c

"$(INTDIR)\mod_jaxer.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\mod_jaxer.rc

"$(INTDIR)\mod_jaxer.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)




!ENDIF 

