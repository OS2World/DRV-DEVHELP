/************************************************************************\
**                                                                      **
**               OS/2(r) Physical Device Driver Libraries               **
**                         for Watcom C/C++ 10                          **
**                                                                      **
**  COPYRIGHT:                                                          **
**                                                                      **
**    (C) Copyright Advanced Gravis Computer Technology Ltd 1994.       **
**        All Rights Reserved.                                          **
**                                                                      **
**  DISCLAIMER OF WARRANTIES:                                           **
**                                                                      **
**    The following [enclosed] code is provided to you "AS IS",         **
**    without warranty of any kind.  You have a royalty-free right to   **
**    use, modify, reproduce and distribute the following code (and/or  **
**    any modified version) provided that you agree that Advanced       **
**    Gravis has no warranty obligations and shall not be liable for    **
**    any damages arising out of your use of this code, even if they    **
**    have been advised of the possibility of such damages.  This       **
**    Copyright statement and Disclaimer of Warranties may not be       **
**    removed.                                                          **
**                                                                      **
\************************************************************************/


// Debug.c
//
// IOStream-like RS-232 output routines for debugging
//
// History:
//
// Sep 30, 94  David Bollo    Initial version



#include "devdebug.h"



// Standard debugging streams

#if defined(DEBUG)

COMStream cdbg =
  {
  0x2f8,                      // Default to COM2
  10,                         // Base 10 (decimal)
  0,                          // Variable width
  USER0,                      // Default debugging level
  ALL                         // Don't mask any
  };

COMStream ccom1 =
  {
  0x3f8,                      // COM1 base port address
  10,                         // Base 10 (decimal)
  0,                          // Variable width
  USER0,                      // Default debugging level
  ALL                         // Don't mask any
  };

COMStream ccom2 =
  {
  0x2f8,                      // COM2 base port address
  10,                         // Base 10 (decimal)
  0,                          // Variable width
  USER0,                      // Default debugging level
  ALL                         // Don't mask any
  };

#endif // DEBUG
