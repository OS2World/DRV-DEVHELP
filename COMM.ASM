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


;; Comm.asm
;;
;; Communication I/O through the RS-232 serial port
;;
;; History:
;;
;; Sep 30, 94  David Bollo    Initial version



;; Allow 386 instructions (OS/2 2.0 or better)

              .386p


_TEXT         segment word public use16 'CODE'
              assume cs:_TEXT, ds:DGROUP, es:NOTHING, ss:NOTHING

              public ComWriteChar
              public ComWriteStr
              public ComWrite32


;; Send a byte through the serial port
;;
;; Arguments:
;;    DX = Base port address for com port (com1 = 3F8, com2 = 2F8)
;;    AH = Byte to send
;; Returns
;;    Carry Clear = success
;;    Carry Set = error
;; Modifies
;;    AL CX SI
ComSendByte   proc near
              mov      si,dx              ;; Save the RS-232 port address
              add      dx,05h             ;; Check the RS-232 line status
              xor      cx,cx              ;; Loop FFFFh times
comsb_wait:   in       al,dx              ;; Get the line status
              test     al,20h             ;; Test the line status
              loopz    comsb_wait         ;; If line not ready, try again
              or       cx,cx              ;; Assume that there was an error
              stc                         ;; If the line never became ready,
              jz       comsb_exit         ;;   then flag an error
              sub      dx,05h             ;; Prepare the RS-232 data port
              mov      al,ah              ;; Load the data to send
              out      dx,al              ;; Send the data
              clc                         ;; Signal success
comsb_exit:   mov      dx,si              ;; Restore the RS-232 port address
              ret                         ;; Exit
ComSendByte   endp


;; Write a character to the serial port
;;
;; Arguments:
;;    DX = Base port address for com port (com1 = 3F8, com2 = 2F8)
;;    AX = Character data
;; Returns
;;    AX = 0 for success, 1 for error
;; Modifies
;;    AX CX SI
ComWriteChar  proc near
              mov      ah,al              ;; Load the character
              call     ComSendByte        ;; Write the character
comchar_exit: setc     al                 ;; Set the return code in ax
              xor      ah,ah              ;;   (0 = success, 1 = error)
              ret                         ;; Exit
ComWriteChar  endp


;; Write a string to the serial port
;;
;; Arguments:
;;    DX = Base port address for com port (com1 = 3F8, com2 = 2F8)
;;    GS:BX = Far pointer to character string
;; Returns
;;    AX = 0 for success, 1 for error
;; Modifies
;;    AX BX CX SI
ComWriteStr   proc near
              clc                         ;; Assume no errors
comstr_loop:  mov      ah,gs:[bx]         ;; Load the current character
              or       ah,ah              ;; If this is the end of the string
              jz       comstr_exit        ;;   then exit
              call     ComSendByte        ;; Write the character
              inc      bx                 ;; Advance to the next byte
              jnc      comstr_loop        ;; Loop
comstr_exit:  setc     al                 ;; Set the return code in ax
              xor      ah,ah              ;;   (0 = success, 1 = error)
              ret                         ;; Exit
ComWriteStr   endp


;; Write an unsigned long to the serial port
;;
;; Arguments:
;;    SI = Base port address for com port (com1 = 3F8, com2 = 2F8)
;;    DX:AX = Unsigned long data
;;    BX = Numeric base (10 = decimal, 16 = hexadecimal)
;;    DI = Width of number to print (0 = variable width)
;; Returns
;;    AX = 0 for success, 1 for error
;; Modifies
;;    EAX EBX CX EDX DI SI
ComWrite32    proc near
;; Split the number into digits pushed onto the stack
              push     bp                 ;; Set up a stack frame
              mov      bp,sp              ;;   (to handle errors more easily)
              xchg     ax,dx              ;; Pack the unsigned long data
              shl      eax,10h            ;;   from dx:ax into a single
              mov      ax,dx              ;;   register (eax)
              and      ebx,0000003Fh      ;; Clear any garbage bits in the base
              xor      cx,cx              ;; Count the number of digits into cx
com32_digit:  inc      cx                 ;; Count one more digit
              cdq                         ;; Determine the current digit into
              div      ebx                ;;   edx and the rest in eax
              cmp      edx,10             ;; If the digit is decimal, then
              jl       com32_dec          ;;   convert it to an ASCII number
              add      dx,'A'-'0'-10      ;;   otherwise, make it a letter
com32_dec:    add      dx,'0'             ;; Store the character in dx
              push     dx                 ;; Save the character
              or       eax,eax            ;; If there are still more digits,
              jnz      com32_digit        ;;   then continue
;; Print any necessary filler digits to adjust width
              mov      dx,si              ;; Load the com port address
              mov      bx,cx              ;; Save the digit count
              or       di,di              ;; If variable width, then
              jz       com32_print        ;;   print the digits
              cmp      di,cx              ;; If the width is greater than the
              jg       com32_pad          ;;   digit count, then zero pad
              sub      cx,di              ;;   otherwise, truncate
              shl      cx,1               ;; Remove the extra digits from the
              add      sp,cx              ;;   stack
              mov      bx,di              ;; Adjust the digit count
              jmp      com32_print
com32_pad:    sub      di,cx              ;; Compute the number of pad digits
              mov      ah,'0'             ;; Pad with zeros
com32_send:   call     ComSendByte        ;; Print a pad digit
              jc       com32_exit         ;; Exit if an error occured
              dec      di                 ;; Loop again to print the next
              jnz      com32_send         ;;   digit
;; Print the digits pulled from the stack
com32_print:  pop      ax                 ;; Pull the next digit from the
              mov      ah,al              ;;   stack
              call     ComSendByte        ;; Print the digit
              jc       com32_exit         ;; Exit if an error occured
              dec      bx                 ;; Loop again to print the next
              jnz      com32_print        ;;   digit
              clc                         ;; Signal a happy exit
;; Clean up, set the result code in ax, and return
com32_exit:   setc     al                 ;; Set the return code in ax
              xor      ah,ah              ;;   (0 = success, 1 = error)
              mov      sp,bp              ;; Restore the stack frame
              pop      bp                 ;;   and the registers
              ret                         ;; Exit
ComWrite32    endp


_TEXT         ends



              end
