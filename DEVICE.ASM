;/************************************************************************\
;**                                                                      **
;**               OS/2(r) Physical Device Driver Libraries               **
;**                         for Watcom C/C++ 10                          **
;**                                                                      **
;**  COPYRIGHT:                                                          **
;**                                                                      **
;**    (C) Copyright Advanced Gravis Computer Technology Ltd 1994.       **
;**        All Rights Reserved.                                          **
;**                                                                      **
;**  DISCLAIMER OF WARRANTIES:                                           **
;**                                                                      **
;**    The following [enclosed] code is provided to you "AS IS",         **
;**    without warranty of any kind.  You have a royalty-free right to   **
;**    use, modify, reproduce and distribute the following code (and/or  **
;**    any modified version) provided that you agree that Advanced       **
;**    Gravis has no warranty obligations and shall not be liable for    **
;**    any damages arising out of your use of this code, even if they    **
;**    have been advised of the possibility of such damages.  This       **
;**    Copyright statement and Disclaimer of Warranties may not be       **
;**    removed.                                                          **
;**                                                                      **
;\************************************************************************/


;; Device.asm
;;
;; Segment declarations and assembler routines for OS/2 PDDs.
;;
;; History:
;;
;; Sep 30, 94  David Bollo    Initial version



;; Declare segments in the correct order

              .386p
              .seq

_HEADER       segment word public use16 'DATA'
_HEADER       ends

CONST         segment word public use16 'DATA'
CONST         ends

CONST2        segment word public use16 'DATA'
CONST2        ends

_DATA         segment word public use16 'DATA'
_DATA         ends

_BSS          segment word public use16 'BSS'
_BSS          ends

_ENDDS        segment word public use16 'ENDDS'
_ENDDS        ends

_INITDATA     segment word public use16 'INITDATA'
_INITDATA     ends


_TEXT         segment word public use16 'CODE'
_TEXT         ends

_ENDCS        segment word public use16 'CODE'
_ENDCS        ends

_INITTEXT     segment word public use16 'CODE'
_INITTEXT     ends


DGROUP        group _HEADER, CONST, CONST2, _DATA, _BSS, _ENDDS, _INITDATA



;; DevHelp support routines and data

_DATA         segment word public use16 'DATA'

              public _Dev_Entry
              public _Dev_StdEntry
              public _Dev_DbgEntry
              public __wcpp_2_pure_error__
              public __wcpp_2_undef_vfun__

_DATA         ends


_TEXT         segment word public use16 'CODE'
              assume cs:CGROUP, ds:DGROUP, es:NOTHING, ss:NOTHING

Entry:
__wcpp_2_pure_error__:
__wcpp_2_undef_vfun__:
DevHelpDebug  proc far
              int 3
              call dword ptr _Dev_StdEntry
              int 3
              ret
DevHelpDebug  endp

_TEXT         ends


_DATA         segment word public use16 'DATA'

_Dev_Entry    dd DevHelpDebug
_Dev_StdEntry dd DevHelpDebug
_Dev_DbgEntry dd DevHelpDebug

_DATA         ends


_ENDDS        segment word public use16 'ENDDS'

              public ___OffFinalDS

___OffFinalDS db 0

_ENDDS        ends


_ENDCS        segment word public use16 'CODE'

              public ___OffFinalCS

___OffFinalCS proc near
              int 3
___OffFinalCS endp

_ENDCS        ends



              end      Entry
