/* Program name:   Controls.C  Title: Designing Custom Controls         */
/* OS/2 Developer Magazine, Issue:  Spring '93, page 72                 */
/* Description:  Custom controls demystified.  Sample image button      */
/*               with text.  Can be used as the basis for other         */
/*               custom controls.                                       */

/* DISCLAIMER OF WARRANTIES:                                            */
/* -------------------------                                            */
/* The following [enclosed] code is sample code created by IBM          */
/* Corporation and Prominare Inc.  This sample code is not part of any  */
/* standard IBM product and is provided to you solely for the purpose   */
/* of assisting you in the development of your applications.  The code  */
/* is provided "AS IS", without warranty of any kind.  Neither IBM nor  */
/* Prominare shall be liable for any damages arising out of your        */
/* use of the sample code, even if they have been advised of the        */
/* possibility of such damages.                                         */

#define INCL_DOS                   /* Include OS/2 DOS Kernal           */
#define INCL_GPI                   /* Include OS/2 PM GPI Interface     */
#define INCL_WIN                   /* Include OS/2 PM Windows Interface */

#include <os2.h>
#include <string.h>

#include "controls.h"

/* This module contains an example installable control that can be used */
/* by any OS/2 2.x Presentation Manager application.  The control       */
/* demonstrates the principles of creating a custom control such        */
/* that new custom controls can be created using this as a model.       */
/*                                                                      */
/* The control can be compiled using the IBM C Set/2 Version 1.0 with   */
/* the following:                                                       */
/*                                                                      */
/*   Icc -G3 -O+ -Rn -W3 -C -FoDriver.Obj Driver.C                      */
/*                                                                      */
/* The control can be compiled using the WATCOM C 386 Version 9.0 with  */
/* the following:                                                       */
/*                                                                      */
/*   Wcl386 -ms -3s -bd -s -ox -zp4 -w3 -c -fo=Driver.Obj Driver.C      */

/* Filename:   Driver.C                                                 */

/*  Version:   1.00                                                     */
/*  Created:   1992-09-02                                               */
/*  Revised:   1992-12-30                                               */
/* Released:   1993-04-22                                               */

/* Routines:   MRESULT EXPENTRY DriverWndProc(HWND hWnd, ULONG msg,     */
/*                                            MPARAM mp1, MPARAM mp2);  */
/*             INT main(INT argc, CHAR *argv[ ]);                       */


/* Copyright ¸ International Business Machines Corp., 1991,1992,1993.   */
/* Copyright ¸ 1989-1993  Prominare Inc.  All Rights Reserved.          */

/* -------------------------------------------------------------------- */

INT main(INT, CHAR *[ ]);
MRESULT EXPENTRY DriverWndProc(HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2);

extern ULONG EXPENTRY InitControls(HAB hAB);

// #pragma subtitle("   Test Driver Window - Client Window Procedure")
// #pragma page( )

/* --- DriverWndProc ---------------------------------- [ Private ] --- */
/*                                                                      */
/*     This function is used to process the messages for the main       */
/*     Client Window.                                                   */
/*                                                                      */
/*     Upon Entry:                                                      */
/*                                                                      */
/*     HWND   hWnd; = Window Handle                                     */
/*     ULONG  msg;  = PM Message                                        */
/*     MPARAM mp1;  = Message Parameter 1                               */
/*     MPARAM mp2;  = Message Parameter 2                               */
/*                                                                      */
/*     Upon Exit:                                                       */
/*                                                                      */
/*     EditWndProc = Message Handling Result                            */
/*                                                                      */
/* -------------------------------------------------------------------- */

MRESULT EXPENTRY DriverWndProc(HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2)

{
HPS   hPS;                         /* Presentation Space Handle         */
RECTL rcl;                         /* Client Window Rectangle           */
LONG  lClr;                        /* Colour Holder                     */

switch ( msg )
   {
                       /* Perform window initialization                 */
   case WM_CREATE :
       lClr = SYSCLR_DIALOGBACKGROUND;
       WinSetPresParam(hWnd, PP_BACKGROUNDCOLORINDEX, 4UL, &lClr);

       WinCreateWindow(hWnd, (PCSZ) "3DFrame", (PCSZ) "", WS_VISIBLE, 10L, 10L, 100L, 100L,
                       hWnd, HWND_TOP, 1024, (PVOID)NULL, (PVOID)NULL);

       WinCreateWindow(hWnd, (PCSZ) "Patterns", (PCSZ) "", WS_VISIBLE | PATSYM_DIAG4, 150L, 10L, 100L, 100L,
                       hWnd, HWND_TOP, 1025, (PVOID)NULL, (PVOID)NULL);

       WinCreateWindow(hWnd, (PCSZ) "3DLine", (PCSZ) "", WS_VISIBLE | LNS_HORIZONTAL, 10L, 200L, 100L, 10L,
                       hWnd, HWND_TOP, 1026, (PVOID)NULL, (PVOID)NULL);

       WinCreateWindow(hWnd, (PCSZ) "3DText", (PCSZ) "3D Text", WS_VISIBLE | DS_RAISED | DT_CENTER | DT_VCENTER,
                       10L, 125L, 100L, 30L,
                       hWnd, HWND_TOP, 1027, (PVOID)NULL, (PVOID)NULL);
       break;
                       /* Erase background                              */

   case WM_ERASEBACKGROUND :
       WinQueryWindowRect(hWnd, &rcl);
       WinFillRect((HPS)mp1, &rcl, SYSCLR_DIALOGBACKGROUND);
       break;
                       /* Paint the main client window                  */
   case WM_PAINT :

       WinFillRect(hPS = WinBeginPaint(hWnd, (HPS)NULL, &rcl), &rcl, SYSCLR_DIALOGBACKGROUND);
       WinEndPaint(hPS);
       break;
                       /* Close Down                                    */
   case WM_CLOSE :

       WinPostMsg(hWnd, WM_QUIT, 0L, 0L);
       break;
                       /* Default message processing                    */
   default:
       return(WinDefWindowProc(hWnd, msg, mp1, mp2));
   }
return(0L);
}
// #pragma subtitle("   Program Controller")
// #pragma page( )

/* --- Main Program Controller ---------------------------------------- */

INT main(INT argc, CHAR *argv[ ])

{
HAB   hAB;                         /* Anchor Block Handle               */
HMQ   hmqDriver;                   /* Message Queue Handle              */
HWND  hwndDriver;                  /* Driver Client Handle              */
HWND  hwndDriverFrame;             /* Driver Frame Handle               */
QMSG  qmsg;                        /* PM Message Queue Holder           */
ULONG flCreateFlags;               /* Window Creation Flags             */

                       /* Initialize the program for PM and create the  */
                       /* message queue                                 */

WinSetCp(hmqDriver = WinCreateMsgQueue(hAB = WinInitialize(0), 0), 850);

                       /* Register the main program window class        */

if ( !WinRegisterClass(hAB, (PCSZ) "Driver", (PFNWP)DriverWndProc,
                       CS_SYNCPAINT | CS_SIZEREDRAW, 0) )
   return(1);
                       /* Initialize the custom controls                */
InitControls(hAB);
                       /* Create the main program window but do not     */
                       /* show it yet                                   */

flCreateFlags = FCF_TITLEBAR | FCF_NOBYTEALIGN | FCF_SYSMENU | FCF_SIZEBORDER |
                FCF_SHELLPOSITION | FCF_TASKLIST;
if ( !(hwndDriverFrame = WinCreateStdWindow(HWND_DESKTOP, WS_VISIBLE,
                                            &flCreateFlags, (PCSZ) "Driver", (PCSZ) "Control Driver", 0L,
                                            (HMODULE)0L, 0L, &hwndDriver)) )
   return(1);
                       /* Retrieve and then dispatch messages           */

while ( WinGetMsg(hAB, &qmsg, (HWND)NULL, 0, 0) )
   WinDispatchMsg(hAB, &qmsg);

WinDestroyWindow(hwndDriverFrame);

WinDestroyMsgQueue(hmqDriver);

                       /* Notify PM that main program thread not needed */
                       /* any longer                                    */
WinTerminate(hAB);

return(0);
}
