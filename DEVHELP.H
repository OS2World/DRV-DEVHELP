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


// DevHelp.h
//
// Watcom C++ callable entry points to the OS/2 DevHlp interface.
//
// All of the DevHlp functions in this library are prefaced with the string
// "Dev" rather than "DevHlp_" which is used by the IBM DDK libraries.  The
// names were made incompatible so that calls to the IBM DevHlp routines
// are not confused with calls to these routines (which may have different
// calling signatures).  If you need the IBM calling signatures, just use
// some inline functions which call the Dev* functions.
//
// Also, the ordering of words in multi-word names has been changed so that
// the object of an operation is the first word, and the operation name is
// the second word.  For example, "DevHlp_AllocPhys" has been renamed to
// "DevPhysAlloc".
//
// This DevHlp library does not include certain categories of DevHlps that
// are available in the IBM DDK.  In particular, ABIOS specific functions
// (function codes 29h and 34h to 37h), monitors (function codes 31h to
// 35h), and some virtual memory functions (function codes 55h to 5Ah and
// 66h) are not (yet) supported.  Also, functions which are no longer
// meaningful under OS/2 2.1, such as UnPhysToVirt and SetRomVector are not
// included either.  However, some undocumented or only partially
// documented DevHlps such as the RAS tracing facility (DevSysTrace,
// function code 28h) are included in this library.
//
// History:
//
// Sep 30, 94  David Bollo    Initial version



// Prevent multiple inclusion
#if !defined(DevHelp_h)
#define DevHelp_h 1

#if !defined(__WATCOMC__) || !defined(__cplusplus)
#error Watcom C++ must be used for the inline DevHlp interface.
#endif


#include "devtype.h"
#include "devrp.h"



// Calling conventions for device driver functions
//
//

// Strategy call calling convention
#pragma aux STRATEGY far parm [es bx];

// Inter device driver calling convention
#pragma aux IDC far loadds parm [] caller reverse value [ax];

// Dynamic API calling convention
#pragma aux DYNAMIC far loadds parm [si] value [ax];

// OS/2 16-bit API calling convention
#pragma aux API16 "^" far parm reverse routine [] \
  value struct float struct caller [] modify [ax bx cx dx];




// Interface to the OS/2 Ring 0 Device Helper functions
//
//

// Private declarations for use by the Dev* library
extern "C"
  {
  extern WORD32 Dev_Entry;
  extern WORD32 Dev_StdEntry;
  extern WORD32 Dev_DbgEntry;
  }



VOID DevInit                  // Initialize the Dev* library (call at Init)
  (
  WORD32 DeviceHelp           // Far pointer to OS/2 Dev_Help entry point
  );                          // Returns: Nothing

inline VOID DevInit(WORD32 DeviceHelp)
  {
  Dev_Entry = Dev_StdEntry = DeviceHelp;
  return;
  }



WORD16 DevDebug               // Set the debugging mode for the Dev* library
  (
  WORD16 EnableInt3           // Debugging flag: 1 = debug on, 0 = debug off
  );                          // Returns: The previous debugging state

inline WORD16 DevDebug(WORD16 EnableInt3)
  {
  WORD16 value = (Dev_Entry == Dev_DbgEntry);
  Dev_Entry = (EnableInt3) ? Dev_DbgEntry : Dev_StdEntry;
  return value;
  }



VOID DevInt3                  // Generate an INT 3 breakpoint
  (
  );                          // Returns: Nothing

#pragma aux DevInt3 =         \
  "int 3";



VOID DevDone                  // Complete device I/O
  (
  RP FAR* RequestPacket       // Request packet for which to unblock threads
  );                          // Returns: Nothing

#pragma aux DevDone =         \
  "mov dl,01h"                \
  "call dword ptr Dev_Entry"  \
  modify [dl]                 \
  parm [es bx];



VOID DevYield                 // Yield the CPU to pending threads
  (
  );                          // Returns: Nothing

#pragma aux DevYield =        \
  "mov dl,02h"                \
  "call dword ptr Dev_Entry"  \
  modify [dl];



VOID DevTCYield               // Yield the CPU to time critical threads
  (
  );                          // Returns: Nothing

#pragma aux DevTCYield =      \
  "mov dl,03h"                \
  "call dword ptr Dev_Entry"  \
  modify [dl];



WORD16 DevBlock               // Block the current thread
  (
  WORD32 Id,                  // Id to be used by DevRun call
  WORD32 WaitMilliSec,        // Milliseconds to wait for (-1 = forever)
  BYTE InterruptableFlag      // 1 if block is interruptable, else 0
  );                          // Returns: 0 = Ran, 1 = Timeout, 2 = Ctrl+C

#pragma aux DevBlock =        \
  "xchg ax,bx"                \
  "xchg di,cx"                \
  "mov dl,04h"                \
  "call dword ptr Dev_Entry"  \
  "setnz al"                  \
  "setc bl"                   \
  "inc al"                    \
  "neg bl"                    \
  "and al,bl"                 \
  "xor ah,ah"                 \
  modify [dl]                 \
  parm [bx ax] [cx di] [dh]   \
  value [ax];



WORD16 DevRun                 // Run a blocked thread
  (
  WORD32 Id                   // Id used in DevBlock call
  );                          // Returns: Count of awakened threads

#pragma aux DevRun =          \
  "xchg ax,bx"                \
  "mov dl,05h"                \
  "call dword ptr Dev_Entry"  \
  modify [dl]                 \
  parm [bx ax]                \
  value [ax];



WORD16 DevSemRequest          // Set a 16 bit semaphore
  (
  HSEMAPHORE Handle,          // Handle of semaphore to request
  WORD32 WaitMilliSec         // Milliseconds to wait for (-1 = forever)
  );                          // Returns: 0 = success, other = error code

#pragma aux DevSemRequest =   \
  "xchg ax,bx"                \
  "xchg di,cx"                \
  "mov dl,06h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  modify [dx]                 \
  parm [bx ax] [cx di]        \
  value [ax];



WORD16 DevSemClear            // Clear a 16 bit semaphore
  (
  HSEMAPHORE Handle           // Handle of semaphore to release
  );                          // Returns: 0 = success, other = error code

#pragma aux DevSemClear =     \
  "xchg ax,bx"                \
  "mov dl,07h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  modify [dx]                 \
  parm [bx ax]                \
  value [ax];



WORD16 DevSemUseHandle        // Obtain a handle to an application semaphore
  (
  WORD32 AppSem,              // Handle of application semaphore
  HSEMAPHORE FAR* Handle      // Storage for converted semaphore handle
  );                          // Returns: 0 = success, other = error code

#pragma aux DevSemUseHandle = \
  "xchg ax,bx"                \
  "mov dx,0108h"              \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "mov es:[si][0],bx"         \
  "mov es:[si][2],ax"         \
  "and ax,dx"                 \
  modify [dx]                 \
  parm [bx ax] [es si]        \
  value [ax];



WORD16 DevSemFreeHandle       // Free a handle obtained with DevSemUseHandle
  (
  HSEMAPHORE Handle           // Handle of system semaphore
  );                          // Returns: 0 = success, other = error code

#pragma aux DevSemFreeHandle= \
  "xchg ax,bx"                \
  "mov dx,0008h"              \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  modify [dx]                 \
  parm [bx ax]                \
  value [ax];



VOID DevReqPush               // Add a request packet to queue
  (
  WORD32 FAR* QueueHead,      // Pointer to space for head of queue (init to 0)
  RP FAR* RequestPacket       // Request packet to add to list
  );                          // Returns: Nothing

#pragma aux DevReqPush =      \
  "mov ax,ds"                 \
  "mov dx,fs"                 \
  "mov ds,dx"                 \
  "mov dl,09h"                \
  "call dword ptr Dev_Entry"  \
  "mov ds,ax"                 \
  modify [ax dx]              \
  parm [fs si] [es bx];



WORD16 DevReqPull             // Remove a request packet from queue
  (
  WORD32 FAR* QueueHead,      // Pointer to head of queue
  RP FAR* FAR* RequestPacket  // Storage for pointer to removed request packet
  );                          // Returns: 0 = success, 1 = queue is empty

#pragma aux DevReqPull =      \
  "mov ax,ds"                 \
  "mov dx,fs"                 \
  "mov ds,dx"                 \
  "mov dl,0Ah"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "mov gs:[di][0],bx"         \
  "mov gs:[di][2],es"         \
  "mov ds,ax"                 \
  modify [es ax bx]           \
  parm [fs si] [gs di]        \
  value [dx];



WORD16 DevReqPullParticular   // Remove a specific request packet from queue
  (
  WORD32 FAR* QueueHead,      // Pointer to head of queue
  RP FAR* RequestPacket       // Request packet to remove from list
  );                          // Returns: 0 = success, 1 = packet not found

#pragma aux DevReqPullParticular= \
  "mov ax,ds"                 \
  "mov dx,fs"                 \
  "mov ds,dx"                 \
  "mov dl,0Bh"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "mov ds,ax"                 \
  modify [ax]                 \
  parm [fs si] [es bx]        \
  value [dx];



VOID DevReqPushSorted         // Add a request packet to queue in sorted order
  (
  WORD32 FAR* QueueHead,      // Pointer to space for head of queue (init to 0)
  RP FAR* RequestPacket       // Request packet to add to list
  );                          // Returns: Nothing

#pragma aux DevReqPushSorted= \
  "mov ax,ds"                 \
  "mov dx,fs"                 \
  "mov ds,dx"                 \
  "mov dl,0Ch"                \
  "call dword ptr Dev_Entry"  \
  "mov ds,ax"                 \
  modify [ax dx]              \
  parm [fs si] [es bx];



WORD16 DevReqAlloc            // Allocate an empty maximum-sized request packet
  (
  BYTE NoWaitFlag,            // 0 to wait for RP or 1 to return immediately
  RP FAR* FAR* RequestPacket  // Storage for pointer to empty request packet
  );                          // Returns: 0 = success, 1 = error

#pragma aux DevReqAlloc =     \
  "mov ax,ds"                 \
  "mov dx,fs"                 \
  "mov ds,dx"                 \
  "mov dl,0Dh"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "mov [di][0],bx"            \
  "mov [di][2],es"            \
  "mov ds,ax"                 \
  modify [es bx]              \
  parm [dh] [fs di]           \
  value [dx];



VOID DevReqFree               // Free a request packet alloced with DevReqAlloc
  (
  RP FAR* RequestPacket       // Request packet to free
  );                          // Returns: Nothing

#pragma aux DevReqFree =      \
  "mov dl,0Eh"                \
  "call dword ptr Dev_Entry"  \
  modify [dl]                 \
  parm [es bx];



VOID DevQueueInit             // Initialize a character queue
  (
  QBASE FAR* Queue            // Pointer to QUEUE<> structure
  );                          // Returns: Nothing

#pragma aux DevQueueInit =    \
  "mov ax,ds"                 \
  "mov dx,fs"                 \
  "mov ds,dx"                 \
  "mov dl,0Fh"                \
  "call dword ptr Dev_Entry"  \
  "mov ds,ax"                 \
  modify [ax dx]              \
  parm [fs bx];



VOID DevQueueFlush            // Empty a character queue
  (
  QBASE FAR* Queue            // Pointer to QUEUE<> structure
  );                          // Returns: Nothing

#pragma aux DevQueueFlush =   \
  "mov ax,ds"                 \
  "mov dx,fs"                 \
  "mov ds,dx"                 \
  "mov dl,10h"                \
  "call dword ptr Dev_Entry"  \
  "mov ds,ax"                 \
  modify [ax dx]              \
  parm [fs bx];



WORD16 DevQueueWrite          // Add a byte to the end of a character queue
  (
  QBASE FAR* Queue,           // Pointer to QUEUE<> structure
  WORD16 Data                 // Data byte to add to queue
  );                          // Returns: 0 = success, 1 = queue full

#pragma aux DevQueueWrite =   \
  "mov cx,ds"                 \
  "mov dx,fs"                 \
  "mov ds,dx"                 \
  "mov dl,11h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "mov ds,cx"                 \
  modify [cx]                 \
  parm [fs bx] [ax]           \
  value [dx];



WORD16 DevQueueRead           // Remove a byte from a character queue
  (
  QBASE FAR* Queue,           // Pointer to QUEUE<> structure
  BYTE FAR* Data              // Storage for data byte
  );                          // Returns: 0 = success, 1 = queue empty

#pragma aux DevQueueRead =    \
  "mov cx,ds"                 \
  "mov dx,fs"                 \
  "mov ds,dx"                 \
  "mov dl,12h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "mov gs:[di],al"            \
  "mov ds,cx"                 \
  modify [al cx]              \
  parm [fs bx] [gs di]        \
  value [dx];



WORD16 DevSegLock             // Lock a segment down in memory
  (
  SEL Selector,               // Selector of memory to lock down
  WORD16 Type,                // Type (0 = short, 1 = long, 3 = long & hi,
                              //       4 = short & verify)
  WORD16 NoWaitFlag,          // 0 to wait for lock or 1 to return immediately
  HLOCK FAR* Handle           // Storage for segment lock handle
  );                          // Returns: 0 = success, 1 = error

#pragma aux DevSegLock =      \
  "mov bh,cl"                 \
  "mov dl,13h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "mov fs:[di][0],bx"         \
  "mov fs:[di][2],ax"         \
  parm [ax] [cx] [bx] [fs di] \
  value [dx];



WORD16 DevSegUnlock           // Unlock a segment locked by DevSegLock
  (
  HLOCK Handle                // Handle of locked segment
  );                          // Returns: 0 = success, other = error code

#pragma aux DevSegUnlock =    \
  "xchg ax,bx"                \
  "mov dl,14h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  modify [dx]                 \
  parm [bx ax]                \
  value [ax];



WORD16 DevPhysToVirt          // Convert a physical address to a 16:16 address
  (
  PHYSICAL PhysAddr,          // Physical address to convert
  WORD16 Length,              // Length of virtual segment
  VIRTUAL FAR* VirtAddr       // Storage for virtual address
  );                          // Returns: 0 = success, other = error code

#pragma aux DevPhysToVirt =   \
  "xchg ax,bx"                \
  "mov dx,0115h"              \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "mov fs:[si][0],di"         \
  "mov fs:[si][2],es"         \
  "and ax,dx"                 \
  modify [dx es di]           \
  parm [bx ax] [cx] [fs si]   \
  value [ax];



VOID DevVirtToPhys            // Convert a locked 16:16 address to physical
  (
  VIRTUAL VirtAddr,           // Virtual address to convert (must be locked)
  PHYSICAL FAR* PhysAddr      // Storage for physical address
  );                          // Returns: Nothing

#pragma aux DevVirtToPhys =   \
  "mov cx,ds"                 \
  "mov ax,es"                 \
  "mov ds,ax"                 \
  "mov dl,16h"                \
  "call dword ptr Dev_Entry"  \
  "mov fs:[di][0],bx"         \
  "mov fs:[di][2],ax"         \
  "mov ds,cx"                 \
  modify [dx ax bx cx]        \
  parm [es si] [fs di];



WORD16 DevPhysToUVirt         // Convert a physical address to an LDT address
  (
  PHYSICAL PhysAddr,          // Physical address to convert
  WORD16 Length,              // Length of virtual segment
  BYTE Type,                  // Type (0 = code, 1 = data, 2 = free, 5 = video)
  BYTE Ring,                  // Ring number for segment (ring 3 or 2)
  VIRTUAL FAR* VirtAddr       // Storage for virtual address
  );                          // Returns: 0 = success, 1 = error

// Massage the Ring and Type parameters into a single value in dh,
// by using a Ring look-up table (bitmask) in dx.
#pragma aux DevPhysToUVirt =  \
  "xchg ax,bx"                \
  "mov dh,30h"                \
  "shl cl,01h"                \
  "shr dh,cl"                 \
  "and dh,03h"                \
  "add dh,ch"                 \
  "mov cx,di"                 \
  "mov dl,17h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "mov fs:[si][0],bx"         \
  "mov fs:[si][2],es"         \
  modify [es]                 \
  parm [bx ax] [di] [ch] [cl] [fs si] \
  value [dx];



WORD16 DevPhysAlloc           // Allocate a block of physical memory
  (
  WORD32 Size,                // Size of memory to allocate
  BYTE LoFlag,                // Flag: 0 = above 1 MB, 1 = below 1 MB
  PHYSICAL FAR* PhysAddr      // Storage for address of memory block
  );                          // Returns: 0 = success, other = error code

#pragma aux DevPhysAlloc =    \
  "xchg ax,bx"                \
  "mov dl,18h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "mov es:[si][0],bx"         \
  "mov es:[si][2],ax"         \
  "and ax,dx"                 \
  modify [dl]                 \
  parm [bx ax] [dh] [es si]   \
  value [ax];



WORD16 DevPhysFree            // Free a block of physical memory
  (
  PHYSICAL PhysAddr           // Address of memory block to be freed
  );                          // Returns: 0 = success, 1 = error

#pragma aux DevPhysFree =     \
  "xchg ax,bx"                \
  "mov dl,19h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  parm [bx ax]                \
  value [dx];



WORD16 DevIRQSet              // Register an interrupt handler for an IRQ
  (
  WORD16 IRQ,                 // IRQ Number to handle
  VOID* Handler,              // Handler function entry-point
  BYTE SharableFlag           // Sharable: 1 = sharable, 0 = exclusive
  );                          // Returns: 0 = success, other = error code

#pragma aux DevIRQSet =       \
  "mov dl,1Bh"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  modify [dl]                 \
  parm [bx] [ax] [dh]         \
  value [ax];



WORD16 DevIRQClear            // Remove a registered IRQ handler
  (
  WORD16 IRQ                  // IRQ Number to release
  );                          // Returns: 0 = success, 1 = error

#pragma aux DevIRQClear =     \
  "mov dl,1Ch"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  parm [bx]                   \
  value [dx];



WORD16 DevTimerSet            // Register a timer handler
  (
  VOID* Handler               // Handler function entry-point
  );                          // Returns: 0 = success, other = error code

#pragma aux DevTimerSet =     \
  "mov dl,1Dh"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  modify [dx]                 \
  parm [ax]                   \
  value [ax];



WORD16 DevTimerClear          // Remove a timer handler
  (
  VOID* Handler               // Handler function entry-point
  );                          // Returns: 0 = success, other = error code

#pragma aux DevTimerClear =   \
  "mov dl,1Eh"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  modify [dx]                 \
  parm [ax]                   \
  value [ax];



WORD16 DevGetDOSVar           // Get a pointer to a system variable
  (
  WORD16 Index,               // ID of variable to request
  VIRTUAL FAR* Pointer        // Storage for address of system variable
  );                          // Returns: 0 = success, 1 = error

#pragma aux DevGetDOSVar =    \
  "mov dl,24h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "mov es:[si][0],bx"         \
  "mov es:[si][2],ax"         \
  modify [bx]                 \
  parm [ax] [es si]           \
  value [dx];



WORD16 DevSendEvent           // Fake a user event to the operating system
  (
  WORD16 Event,               // ID of the event to fake
  WORD16 Parameter            // Parameter for the event
  );                          // Returns: 0 = success, 1 = error

#pragma aux DevSendEvent =    \
  "mov dl,25h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  parm [ax] [bx]              \
  value [dx];



WORD16 DevVerifyAccess        // Verify that the caller has access to memory
  (
  VIRTUAL Address,            // Address of memory to verify
  WORD16 Length,              // Length of memory to verify
  BYTE Type                   // Type of access (0 = read, 1 = read/write)
  );                          // Returns: 0 = success, 1 = error

#pragma aux DevVerifyAccess = \
  "xchg ax,di"                \
  "mov dl,27h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  modify [dl]                 \
  parm [di ax] [cx] [dh]      \
  value [dx];



WORD16 DevSysTrace            // Trace data into the RAS tracing facility
  (
  WORD16 MajorCode,           // Major trace code
  WORD16 MinorCode,           // Minor trace code
  WORD16 Length,              // Length of data buffer
  BYTE FAR* Data              // Data buffer
  );                          // Returns: 0 = success, 1 = error

#pragma aux DevSysTrace =     \
  "mov di,ds"                 \
  "mov dx,fs"                 \
  "mov ds,dx"                 \
  "mov dl,28h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "mov ds,di"                 \
  modify [di]                 \
  parm [ax] [cx] [bx] [fs si] \
  value [dx];



WORD16 DevAttachDD            // Attach to another device driver for IDC
  (
  CHAR* DriverName,           // Device driver name (from device header)
  IDCDATA* IDCData            // Storage for IDC data
  );                          // Returns: 0 = success, 1 = error

#pragma aux DevAttachDD =     \
  "mov dl,2Ah"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  parm [bx] [di]              \
  value [dx];



VOID DevInternalError         // Signal an internal consistency error to OS/2
  (
  CHAR FAR* Message,          // Text describing error
  WORD16 Length               // Length of message text
  );                          // Returns: Function does not return

#pragma aux DevInternalError= \
  "mov ax,ds"                 \
  "mov dx,fs"                 \
  "mov ds,dx"                 \
  "mov dl,2Bh"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "mov ds,ax"                 \
  modify [ax]                 \
  parm [fs si] [di]           \
  value [dx];



WORD16 DevAllocGDTSel         // Allocate GDT selectors at Init time
  (
  SEL FAR* SelectorArray,     // Storage for allocated GDT selectors
  WORD16 Count                // Number of GDT selectors to allocate
  );                          // Returns: 0 = success, other = error code

#pragma aux DevAllocGDTSel =  \
  "mov dl,2Dh"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  modify [dx]                 \
  parm [es di] [cx]           \
  value [ax];



WORD16 DevPhysToGDTSel        // Convert a 32 bit address to a GDT selector
  (
  PHYSICAL Address,           // 32 bit physical address to convert
  WORD16 Length,              // Length of fabricated segment
  SEL Selector                // Selector to refer to 32 bit address
  );                          // Returns: 0 = success, other = error code

#pragma aux DevPhysToGDTSel = \
  "xchg ax,bx"                \
  "mov dl,2Eh"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  modify [dx]                 \
  parm [bx ax] [cx] [si]      \
  value [ax];



VOID DevEOI                   // Signal end of interrupt processing to PIC
  (
  WORD16 IRQ                  // IRQ number to end
  );                          // Returns: Nothing

#pragma aux DevEOI =          \
  "mov dl,31h"                \
  "call dword ptr Dev_Entry"  \
  modify [dl]                 \
  parm [ax];



WORD16 DevTimerModify         // Modify or create periodic timer handler
  (
  VOID* Handler,              // Handler function entry-point
  WORD16 Period               // Period between calls (number of timer ticks)
  );                          // Returns: 0 = success, other = error code

#pragma aux DevTimerModify =  \
  "mov dl,33h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  modify [dx]                 \
  parm [ax] [bx]              \
  value [ax];



WORD16 DevIRQStackUsage       // Register stack usage properties
  (
  STACKUSAGE FAR* StackUsage  // Stack usage information
  );                          // Returns: 0 = success, 1 = error

#pragma aux DevIRQStackUsage= \
  "mov ax,ds"                 \
  "mov dx,fs"                 \
  "mov ds,dx"                 \
  "mov dl,3Ah"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "mov ds,ax"                 \
  modify [ax]                 \
  parm [fs bx]                \
  value [dx];



VOID DevRegisterPDD           // Register driver for PDD-VDD communications
  (
  CHAR FAR* Name,             // Device driver name
  VOID FAR* EntryPoint        // Entry point for PDD-VDD communications
  );                          // Returns: Nothing = success, halt = error

#pragma aux DevRegisterPDD =  \
  "mov ax,ds"                 \
  "mov dx,fs"                 \
  "mov ds,dx"                 \
  "mov dl,50h"                \
  "call dword ptr Dev_Entry"  \
  "mov ds,ax"                 \
  modify [ax dx]              \
  parm [fs si] [es di];



WORD16 DevBeep                // Generate a beep
  (
  WORD16 Frequency,           // Beep pitch in hz
  WORD16 Duration             // Beep duration
  );                          // Returns: 0 = success, other = error code

#pragma aux DevBeep =         \
  "mov dl,52h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  modify [dx]                 \
  parm [bx] [cx]              \
  value [ax];



WORD16 DevFreeGDTSel          // Free an allocated GDT selector at task time
  (
  SEL Selector                // Selector to free
  );                          // Returns: 0 = success, other = error code

#pragma aux DevFreeGDTSel =   \
  "mov dl,53h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  modify [dx]                 \
  parm [ax]                   \
  value [ax];



WORD16 DevVirtToLin           // Convert a virtual address to linear
  (
  SEL Selector,               // Selector to convert
  WORD32 Offset,              // Offset to convert
  LINEAR FAR* Linear          // Storage for linear address
  );                          // Returns: 0 = success, other = error code

#pragma aux DevVirtToLin =    \
  "xchg si,dx"                \
  "shl esi,10h"               \
  "mov si,dx"                 \
  "mov dl,5Bh"                \
  "call dword ptr Dev_Entry"  \
  "mov es:[di],eax"           \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  parm [ax] [dx si] [es di]   \
  value [ax];



WORD16 DevLinToGDTSel         // Convert a linear address to virtual
  (
  SEL Selector,               // Selector to refer to linear address
  LINEAR Linear,              // Linear address to convert
  WORD32 Length               // Length of memory space to convert
  );                          // Returns: 0 = success, other = error code

#pragma aux DevLinToGDTSel =  \
  "xchg bx,di"                \
  "shl ebx,10h"               \
  "mov bx,di"                 \
  "shl ecx,10h"               \
  "mov cx,dx"                 \
  "mov dl,5Ch"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  parm [ax] [di bx] [cx dx]   \
  value [ax];



WORD16 DevCtxAllocate         // Allocate a context hook
  (
  VOID FAR* Handler,          // Pointer to handler function
  HCONTEXT FAR* Hook          // Storage for allocated context hook handle
  );                          // Returns: 0 = success, other = error code

#pragma aux DevCtxAllocate =  \
  "mov cx,ds"                 \
  "mov dx,fs"                 \
  "mov ds,dx"                 \
  "and eax,0000FFFFh"         \
  "mov ebx,0FFFFFFFFh"        \
  "mov dl,63h"                \
  "call dword ptr Dev_Entry"  \
  "mov es:[di],eax"           \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  "mov ds,cx"                 \
  modify [cx dx bx]           \
  parm [fs ax] [es di]        \
  value [ax];



WORD16 DevCtxFree             // Free a context hook
  (
  HCONTEXT Hook               // Handle of context hook to free
  );                          // Returns: 0 = success, other = error code

#pragma aux DevCtxFree =      \
  "shl eax,10h"               \
  "mov ax,dx"                 \
  "mov dl,64h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  parm [ax dx]                \
  value [ax];



WORD16 DevCtxArm              // Arm a context hook
  (
  HCONTEXT Hook,              // Handle of context hook to arm
  WORD32 Parameter            // Parameter to pass to context hook in EAX
  );                          // Returns: 0 = success, other = error code

#pragma aux DevCtxArm =       \
  "xchg bx,cx"                \
  "xchg ax,dx"                \
  "shl eax,10h"               \
  "mov ax,dx"                 \
  "shl ebx,10h"               \
  "mov bx,cx"                 \
  "mov ecx,0FFFFFFFFh"        \
  "mov dl,65h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  parm [cx bx] [dx ax]        \
  value [ax];



WORD16 DevEventOpen           // Open a shared event semaphore
  (
  WORD32 Semaphore            // Shared event semaphore handle
  );                          // Returns: 0 = success, other = error code

#pragma aux DevEventOpen =    \
  "xchg ax,dx"                \
  "shl eax,10h"               \
  "mov ax,dx"                 \
  "mov dl,67h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  parm [dx ax]                \
  value [ax];



WORD16 DevEventClose          // Close a shared event semaphore
  (
  WORD32 Semaphore            // Shared event semaphore handle
  );                          // Returns: 0 = success, other = error code

#pragma aux DevEventClose =   \
  "xchg ax,dx"                \
  "shl eax,10h"               \
  "mov ax,dx"                 \
  "mov dl,68h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  parm [dx ax]                \
  value [ax];



WORD16 DevEventPost           // Post a shared event semaphore
  (
  WORD32 Semaphore            // Shared event semaphore handle
  );                          // Returns: 0 = success, other = error code

#pragma aux DevEventPost =    \
  "xchg ax,dx"                \
  "shl eax,10h"               \
  "mov ax,dx"                 \
  "mov dl,69h"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  parm [dx ax]                \
  value [ax];



WORD16 DevEventReset          // Reset a shared event semaphore
  (
  WORD32 Semaphore            // Shared event semaphore handle
  );                          // Returns: 0 = success, other = error code

#pragma aux DevEventReset =   \
  "xchg ax,dx"                \
  "shl eax,10h"               \
  "mov ax,dx"                 \
  "mov dl,6Ah"                \
  "call dword ptr Dev_Entry"  \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  parm [dx ax]                \
  value [ax];



WORD16 DevDynamicAPI16        // Create a call gate into a 16:16 function
  (
  VOID FAR* EntryPoint,       // Entry point for the created call gate
  WORD16 Count,               // Parameter count
  BYTE GateType,              // Gate type: 1 = 16 bit gate, 0 = 32 bit gate
  SEL FAR* Selector           // Storage for created call gate selector
  );                          // Returns: 0 = success, other = error code

#pragma aux DevDynamicAPI16 = \
  "xchg ax,bx"                \
  "and dh,01h"                \
  "or dh,02h"                 \
  "mov dl,6Ch"                \
  "call dword ptr Dev_Entry"  \
  "mov es:[si],di"            \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  modify [di dl]              \
  parm [bx ax] [cx] [dh] [es si] \
  value [ax];



WORD16 DevDynamicAPI32        // Create a call gate into a 0:32 function
  (
  LINEAR EntryPoint,          // Entry point for the created call gate
  WORD16 Count,               // Parameter count
  BYTE GateType,              // Gate type: 1 = 16 bit gate, 0 = 32 bit gate
  SEL FAR* Selector           // Storage for created call gate selector
  );                          // Returns: 0 = success, other = error code

#pragma aux DevDynamicAPI32 = \
  "xchg ax,bx"                \
  "and dh,01h"                \
  "mov dl,6Ch"                \
  "call dword ptr Dev_Entry"  \
  "mov es:[si],di"            \
  "setc dl"                   \
  "xor dh,dh"                 \
  "neg dx"                    \
  "and ax,dx"                 \
  modify [di dl]              \
  parm [bx ax] [cx] [dh] [es si] \
  value [ax];




// Interface to the OS/2 Ring 3 APIs available at initialization
//
//

extern "C"
{
//File bsedos.h:
   #define FILE_NORMAL     0x0000
//File bsedos.h:
   #define FILE_ARCHIVED   0x0020
   #define MUST_HAVE_ARCHIVED      ( (FILE_ARCHIVED  << 8) | FILE_ARCHIVED  )
//File bsedos.h:
   #define OPEN_ACTION_CREATE_IF_NEW      0x0010  /* ---- ---- 0001 ---- */
//File bsedos.h:
   #define OPEN_ACTION_OPEN_IF_EXISTS     0x0001  /* ---- ---- ---- 0001 */
//File bsedos.h:
   #define OPEN_FLAGS_NOINHERIT           0x0080  /* ---- ---- 1--- ---- */
//File bseerr.h:
   #define NO_ERROR        0       /* MSG%RESPONSE_DATA */
   #define NO_ERROR_MOUSE_NO_DATA  393     /* MSG%none */
   #define NO_ERROR_MOUSE_PTR_DRAWN        394     /* MSG%none */
   #define NO_ERROR_BIDI_RW_INCOMPLETE     684    /* MSG%none */
//File bsedos.h:
   #define FILE_BEGIN      0x0000   /* Move relative to beginning of file */
//File bsedos.h:
   #define OPEN_SHARE_DENYNONE            0x0040  /* ---- ---- -100 ---- */
//File bsedos.h:
   #define OPEN_ACCESS_READWRITE          0x0002  /* ---- ---- ---- -010 */

WORD16 DosBeep(WORD16 Frequency, WORD16 Duration);
#pragma aux (API16) DosBeep;

WORD16 DosCaseMap(WORD16 Length, VOID FAR* Country, CHAR FAR* String);
#pragma aux (API16) DosCaseMap;

WORD16 DosChgFilePtr(HFILE File, LONG Dist, WORD16 Type, WORD32 FAR* Pointer);
#pragma aux (API16) DosChgFilePtr;

WORD16 DosClose(HFILE File);
#pragma aux (API16) DosClose;

WORD16 DosDelete(CONST CHAR FAR* FileName, WORD32 Reserved);
#pragma aux (API16) DosDelete;

WORD16 DosDevConfig(BYTE FAR* Info, WORD16 Type, WORD16 Reserved);
#pragma aux (API16) DosDevConfig;

WORD16 DosDevIOCtl(VOID FAR*, VOID FAR*, WORD16, WORD16, WORD16);
#pragma aux (API16) DosDevIOCtl;

WORD16 DosFindClose(HDIR Handle);
#pragma aux (API16) DosFindClose;

WORD16 DosFindFirst(CONST CHAR FAR* FileName, HDIR FAR* Handle, WORD16 Attrib,
                    VOID FAR* ResultBuf, WORD16 ResultLen,
                    WORD32 FAR* SearchCount, WORD32 Level);
#pragma aux (API16) DosFindFirst;

WORD16 DosFindNext(HDIR Handle, VOID FAR* ResultBuf, WORD16 ResultLen,
                   WORD32 FAR* SearchCount);
#pragma aux (API16) DosFindNext;

WORD16 DosGetEnv(VOID FAR*, VOID FAR*);
#pragma aux (API16) DosGetEnv;

WORD16 DosGetMessage(WORD32 FAR* Table, WORD16 Count,
                     CHAR FAR* DataBuf, WORD16 DataSize,
                     WORD16 Number, CONST CHAR FAR* FileName,
                     WORD32 FAR* Length);
#pragma aux (API16) DosGetMessage;

WORD16 DosOpen(CONST CHAR FAR* FileName, HFILE FAR* File, WORD32 FAR* Action,
               WORD32 Size, WORD16 Attr, WORD16 Flag, WORD16 Mode, WORD32);
#pragma aux (API16) DosOpen;

WORD16 DosPutMessage(HFILE, WORD16 Length, CONST CHAR FAR* Message);
#pragma aux (API16) DosPutMessage;

WORD16 DosQCurDir(WORD16 Drive, CONST CHAR FAR* Path, WORD32 FAR* Length);
#pragma aux (API16) DosQCurDir;

WORD16 DosQCurDisk(WORD32 FAR* Drive, WORD32 FAR* DriveMap);
#pragma aux (API16) DosQCurDisk;

WORD16 DosQFileInfo(HFILE File, WORD16 Level, VOID FAR* Buffer, WORD16 Size);
#pragma aux (API16) DosQFileInfo;

WORD16 DosQFileMode(VOID FAR*, VOID FAR*, WORD32);
#pragma aux (API16) DosQFileMode;

WORD16 DosRead(HFILE File, VOID FAR* Buffer, WORD16 Size, WORD16 FAR* Length);
#pragma aux (API16) DosRead;

WORD16 DosWrite(HFILE File, VOID FAR* Buffer, WORD16 Size, WORD16 FAR* Length);
#pragma aux (API16) DosWrite;
}




#endif // DevHelp_h
