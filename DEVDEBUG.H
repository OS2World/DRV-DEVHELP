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


// DevDebug.h
//
// IOStream-like RS-232 output routines for debugging
//
// History:
//
// Sep 30, 94  David Bollo    Initial version



// Prevent multiple inclusion
#if !defined(DevDebug_h)
#define DevDebug_h 1

#if !defined(__WATCOMC__) || !defined(__cplusplus)
#error Watcom C++ must be used for the RS-232 debugging library.
#endif


#include "devtype.h"



// Interface to the assembler IO routines

WORD16 ComWriteChar(WORD16 Port, WORD16 Char);
#pragma aux ComWriteChar "ComWriteChar"               \
  parm [dx] [ax]                                      \
  value [ax]                                          \
  modify exact [ax cx si];

WORD16 ComWriteStr(WORD16 Port, const CHAR FAR* String);
#pragma aux ComWriteStr "ComWriteStr"                 \
  parm [dx] [gs bx]                                   \
  value [ax]                                          \
  modify exact [ax bx cx si];

WORD16 ComWrite32(WORD16 Port, WORD32 Number, WORD16 Base, WORD16 Width);
#pragma aux ComWrite32 "ComWrite32"                   \
  parm [si] [dx ax] [bx] [di]                         \
  value [ax]                                          \
  modify exact [ax bx cx dx di si];



// COM Port debugging stream
//
// This is a structure (rather than a class) so that it can be statically
// initialized at compile time.  If it had a constructor, then Watcom
// would generate a function call which is expected to be called when the
// runtime library starts up; however, because the runtime is not being
// included, the constructor would never get called.
//
// These routines are only defined if DEBUG is defined at the time that
// this header file is processed.  Otherwise, all of the inline functions
// are defined as empty functions, so that production-level code is
// generated.

// Stream type (public so that it can be statically initialized)
struct COMStream
  {
  WORD16      Port;           // Base port address
  WORD16      Base;           // Current numeric base setting
  WORD16      Width;          // Current field width (integers only)
  WORD16      Level;          // Current debugging level
  WORD16      Mask;           // Debugging level mask
  };

// Types for internal use by manipulators
struct COMBase  { WORD16 Value; };
struct COMWidth { WORD16 Value; };
struct COMMask  { WORD16 Value; };
enum   COMEndl  { endl };
enum   COMLevel
         {
         ALL = 0xFFFF,
         NONE = 0x0000,
         FATAL = 0x8000,
         ERROR = 0x4000,
         WARNING = 0x2000,
         SKELETON = 0x1000,
         OPERATION = 0x0800,
         VARIABLE = 0x0400,
         USER9 = 0x0200,
         USER8 = 0x0100,
         USER7 = 0x0080,
         USER6 = 0x0040,
         USER5 = 0x0020,
         USER4 = 0x0010,
         USER3 = 0x0008,
         USER2 = 0x0004,
         USER1 = 0x0002,
         USER0 = 0x0001,
         };

// Aliases for standard bases
#define       oct             setb(8)
#define       dec             setb(10)
#define       hex             setb(16)


#if defined(DEBUG)

inline COMBase setb(WORD16 b = 10)
  {
  COMBase cb;
  cb.Value = b;
  return cb;
  }

inline COMWidth setw(WORD16 w = 0)
  {
  COMWidth cw;
  cw.Value = w;
  return cw;
  }

inline COMMask setm(WORD16 m = ALL)
  {
  COMMask cm;
  cm.Value = m;
  return cm;
  }

inline COMStream& operator<<(COMStream& cs, COMBase b)
  {
  cs.Base = b.Value;
  return cs;
  }

inline COMStream& operator<<(COMStream& cs, COMWidth w)
  {
  cs.Width = w.Value;
  return cs;
  }

inline COMStream& operator<<(COMStream& cs, COMMask m)
  {
  cs.Mask = m.Value;
  return cs;
  }

inline COMStream& operator<<(COMStream& cs, COMLevel l)
  {
  cs.Level = l;
  return cs;
  }

inline COMStream& operator<<(COMStream& cs, COMEndl)
  {
  if (!(cs.Level & cs.Mask)) return cs;
  ComWriteChar(cs.Port, '\r');
  ComWriteChar(cs.Port, '\n');
  return cs;
  }

inline COMStream& operator<<(COMStream& cs, const CHAR FAR* s)
  {
  if (!(cs.Level & cs.Mask)) return cs;
  ComWriteStr(cs.Port, s);
  return cs;
  }

inline COMStream& operator<<(COMStream& cs, CHAR c)
  {
  if (!(cs.Level & cs.Mask)) return cs;
  ComWriteChar(cs.Port, c);
  return cs;
  }

inline COMStream& operator<<(COMStream& cs, unsigned short i)
  {
  if (!(cs.Level & cs.Mask)) return cs;
  ComWrite32(cs.Port, i, cs.Base, cs.Width);
  return cs;
  }

inline COMStream& operator<<(COMStream& cs, signed short i)
  {
  if (!(cs.Level & cs.Mask)) return cs;
  WORD16 width = cs.Width;
  if (cs.Base == 10 && i < 0)
    {
    ComWriteChar(cs.Port, '-');
    i = -i;
    if (width == 1)
      return cs;
    if (width != 0)
      width--;
    }
  ComWrite32(cs.Port, i, cs.Base, width);
  return cs;
  }

inline COMStream& operator<<(COMStream& cs, unsigned int i)
  {
  if (!(cs.Level & cs.Mask)) return cs;
  ComWrite32(cs.Port, i, cs.Base, cs.Width);
  return cs;
  }

inline COMStream& operator<<(COMStream& cs, signed int i)
  {
  if (!(cs.Level & cs.Mask)) return cs;
  WORD16 width = cs.Width;
  if (cs.Base == 10 && i < 0)
    {
    ComWriteChar(cs.Port, '-');
    i = -i;
    if (width == 1)
      return cs;
    if (width != 0)
      width--;
    }
  ComWrite32(cs.Port, i, cs.Base, width);
  return cs;
  }

inline COMStream& operator<<(COMStream& cs, unsigned long i)
  {
  if (!(cs.Level & cs.Mask)) return cs;
  ComWrite32(cs.Port, i, cs.Base, cs.Width);
  return cs;
  }

inline COMStream& operator<<(COMStream& cs, signed long i)
  {
  if (!(cs.Level & cs.Mask)) return cs;
  WORD16 width = cs.Width;
  if (cs.Base == 10 && i < 0)
    {
    ComWriteChar(cs.Port, '-');
    i = -i;
    if (width == 1)
      return cs;
    if (width != 0)
      width--;
    }
  ComWrite32(cs.Port, i, cs.Base, width);
  return cs;
  }

#else // DEBUG

inline COMEndl setb(WORD16 = 10)                              { return endl; }
inline COMEndl setw(WORD16 = 0)                               { return endl; }
inline COMEndl setm(WORD16 = ALL)                             { return endl; }
inline COMStream& operator<<(COMStream& cs, COMBase)          { return cs; }
inline COMStream& operator<<(COMStream& cs, COMWidth)         { return cs; }
inline COMStream& operator<<(COMStream& cs, COMMask)          { return cs; }
inline COMStream& operator<<(COMStream& cs, COMLevel)         { return cs; }
inline COMStream& operator<<(COMStream& cs, COMEndl)          { return cs; }
inline COMStream& operator<<(COMStream& cs, const CHAR FAR*)  { return cs; }
inline COMStream& operator<<(COMStream& cs, CHAR)             { return cs; }
inline COMStream& operator<<(COMStream& cs, unsigned short)   { return cs; }
inline COMStream& operator<<(COMStream& cs, signed short)     { return cs; }
inline COMStream& operator<<(COMStream& cs, unsigned int)     { return cs; }
inline COMStream& operator<<(COMStream& cs, signed int)       { return cs; }
inline COMStream& operator<<(COMStream& cs, unsigned long)    { return cs; }
inline COMStream& operator<<(COMStream& cs, signed long)      { return cs; }

#endif // DEBUG



// Standard debugging streams

extern COMStream cdbg;        // Standard debugging stream
extern COMStream ccom1;       // COM Port 1
extern COMStream ccom2;       // COM Port 2



#endif // DevDebug_h
