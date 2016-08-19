#/************************************************************************\
#**                                                                      **
#**               OS/2(r) Physical Device Driver Libraries               **
#**                         for Watcom C/C++ 10                          **
#**                                                                      **
#**  COPYRIGHT:                                                          **
#**                                                                      **
#**    (C) Copyright Advanced Gravis Computer Technology Ltd 1994.       **
#**        All Rights Reserved.                                          **
#**                                                                      **
#**  DISCLAIMER OF WARRANTIES:                                           **
#**                                                                      **
#**    The following [enclosed] code is provided to you "AS IS",         **
#**    without warranty of any kind.  You have a royalty-free right to   **
#**    use, modify, reproduce and distribute the following code (and/or  **
#**    any modified version) provided that you agree that Advanced       **
#**    Gravis has no warranty obligations and shall not be liable for    **
#**    any damages arising out of your use of this code, even if they    **
#**    have been advised of the possibility of such damages.  This       **
#**    Copyright statement and Disclaimer of Warranties may not be       **
#**    removed.                                                          **
#**                                                                      **
#\************************************************************************/


#===================================================================
#
#   Device Library Make File
#
#   History:
#
#   Sep 30, 1994 David Bollo  Initial version
#
#===================================================================



#===================================================================
#
#   Auto-dependency information
#
#===================================================================
.ERASE
.SUFFIXES:
.SUFFIXES: .lst .obj .lib .c .asm .def

CFLAGS  = -dDEBUG=1 -bt=os2 -d1 -e60 -5 -i$(INC) -omerlin -s -wx -xd -ze -zdp -zff -zgf -zl -zq -zu
CC      = WPP $(CFLAGS)

AFLAGS  = -q
ASM     = WASM $(AFLAGS)

DFLAGS  = -l -s
DIS     = WDISASM $(DFLAGS)

LFLAGS  = system os2 dll option quiet, map, align=512, eliminate
LINK    = WLINK $(LFLAGS)

BFLAGS  = -s -t -c -b -q -n
LIB     = WLIB $(BFLAGS)

IFLAGS  = /nologo
IMPLIB  = IMPLIB $(IFLAGS)

.obj.lst:
        $(DIS) $*

.c.obj: .AUTODEPEND
        $(CC) $*.c

.asm.obj: .AUTODEPEND
        $(ASM) $*.asm

.def.lib:
        $(IMPLIB) $*.lib $*.def



#===================================================================
#
#   List of source files
#
#===================================================================
FILE0   = device.obj impdos.lib impmsg.lib impnls.lib
FILE1   = math.obj comm.obj debug.obj
FILE2   =
FILE3   =
FILE4   =
FILE5   =
FILE6   =
FILE7   =
FILE8   =
FILE9   =
FILES   = $(FILE0) $(FILE1) $(FILE2) $(FILE3) $(FILE4) $(FILE5) $(FILE6) $(FILE7) $(FILE8) $(FILE9)



#===================================================================
#
#   Specific dependencies
#
#===================================================================
all: device.lib devmisc.lib .symbolic
    @%null

device.lib: $(FILE0)
    $(LIB) device.lib +device.obj +impdos.lib +impmsg.lib +impnls.lib

devmisc.lib: $(FILE1)
    $(LIB) devmisc.lib +math.obj +comm.obj +debug.obj
