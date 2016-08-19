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


// DevType.h
//
// Type definitions for OS/2 2.x physical device drivers.
//
// History:
//
// Sep 30, 94  David Bollo    Initial version


// Prevent multiple inclusion
#if !defined(DevType_h)
#define DevType_h 1

#if !defined(__WATCOMC__) || !defined(__cplusplus)
#error Watcom C++ must be used for strict type checking.
#endif


// Match OS/2's layout model for structures
#pragma pack(1);
#define FAR far


// Standard types
#define       CONST           const
typedef       void            VOID;
typedef       char            CHAR;
typedef       short           SHORT;
typedef       long            LONG;
typedef       unsigned char   BYTE;
typedef       unsigned short  WORD16;
typedef       unsigned long   WORD32;


// Pointer type for physical addresses
typedef       WORD32          PHYSICAL;

// Pointer type for linear addresses
typedef       WORD32          LINEAR;

// Pointer types for virtual addresses
typedef       WORD16          SEGMENT;
typedef       WORD16          OFFSET;
typedef       VOID FAR*       VIRTUAL;

// Selector type for local and global descriptor tables
typedef       WORD16          SEL;

// System file handle type
typedef       WORD16          HFILE;

// File search session handle type
typedef       WORD16          HDIR;

// Locked segment handle type
typedef       WORD32          HLOCK;

// Context hook handle type
typedef       WORD32          HCONTEXT;

// Semaphore handle type for system semaphores
typedef       WORD32          HSEMAPHORE;

// Character queues
struct QBASE
  {
  WORD16      Size;           // Size of queue (in bytes)
  WORD16      Index;          // Index of next byte out
  WORD16      Count;          // Count of bytes in queue
  };
template <int s> struct QUEUE : public QBASE
  {
  BYTE        Buffer[s];      // Queue buffer
  };

// Inter device driver communication structure
struct        IDCDATA
  {
  WORD16      Reserved1;      // Reserved (formerly real mode offset)
  WORD16      Reserved2;      // Reserved (formerly real mode code segment)
  WORD16      Reserved3;      // Reserved (formerly real mode data segment)
  OFFSET      Offset;         // Offset of IDC entry point
  SEGMENT     Segment;        // Segment of IDC entry point
  SEGMENT     Data;           // Data segment of IDC device driver
  };

// Stack usage information
struct        STACKUSAGE
  {
  WORD16      Size;           // Size of this structure = sizeof(STACKUSAGE)
  WORD16      Flags;          // Flags: bit 1 on = driver enables interrupts
  WORD16      IRQ;            // IRQ number for this stack information
  WORD16      CLIStack;       // Bytes of stack used while interrupts are clear
  WORD16      STIStack;       // Bytes of stack used while interrupts are set
  WORD16      EOIStack;       // Bytes of stack used after EOI is issued
  WORD16      NestingLevel;   // Maximum number of times to nest interrupt
  };

// Device driver header
//
// Instances of this structure should be placed in the header segment to
// ensure that they are located at the beginning of the device driver
// file.
struct        DEVHEADER
  {
  DEVHEADER FAR* Link;        // Link to next header in chain
  WORD16      DAWFlags;       // Device attribute word
  VOID*       StrategyEntry;  // Entry point to strategy routine
  VOID*       IDCEntry;       // Entry point to IDC routine
  CHAR        Name[8];        // Device driver name
  CHAR        Reserved[8];    // Reserved
  WORD32      Capabilities;   // Capabilities bit strip (for level 3 DDs)
  };

// Constant for final DEVHEADER in chain
#define       FinalLink       ((DEVHEADER FAR*)0xFFFFFFFFul)

// Constants for device attribute word
const WORD16  DAW_CHARACTER = 0x8000;     // Character device
const WORD16  DAW_IDC       = 0x4000;     // IDC aware device
const WORD16  DAW_NONIBM    = 0x2000;     // Non-IBM Block device
const WORD16  DAW_SHARE     = 0x1000;     // Sharable device
const WORD16  DAW_OPEN      = 0x0800;     // Requires open and close requests
const WORD16  DAW_LEVEL3    = 0x0180;     // Level 3 device
const WORD16  DAW_LEVEL2    = 0x0100;     // Level 2 device
const WORD16  DAW_LEVEL1    = 0x0080;     // Level 1 device
const WORD16  DAW_CLOCK     = 0x0008;     // Clock device
const WORD16  DAW_NUL       = 0x0004;     // Nul device
const WORD16  DAW_STDOUT    = 0x0002;     // Standard output device
const WORD16  DAW_STDIN     = 0x0001;     // Standard input device

// Constants for capabilities bit strip (used by level 3 devices)
const WORD32  CAP_COMPLETE  = 0x00000010; // Supports Init Complete request
const WORD32  CAP_ADD       = 0x00000008; // Participates in ADD strategy
const WORD32  CAP_PARALLEL  = 0x00000004; // Supports parallel ports
const WORD32  CAP_32BIT     = 0x00000002; // Supports addressing above 16MB
const WORD32  CAP_SHUTDOWN  = 0x00000001; // Supports IOCtl2 and shutdown

// Offsets for the end of the resident code and data segments
#define OffsetFinalCS         ((WORD16)&__OffFinalCS)
#define OffsetFinalDS         ((WORD16)&__OffFinalDS)
extern "C"
  {
  extern BYTE __OffFinalCS;               // End of resident code
  extern BYTE __OffFinalDS;               // End of resident data
  }


#endif // DevType_h
