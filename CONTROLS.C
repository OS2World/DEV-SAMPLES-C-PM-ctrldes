/* Program name:   Controls.C  Title: Designing	Custom Controls		*/
/* OS/2	Developer Magazine, Issue:  Spring '93, page 72                 */
/* Description:	 Custom	controls demystified.  Sample custom controls.	*/
/*		 Can be	used as	the basis for other custom controls.	*/

/* DISCLAIMER OF WARRANTIES:						*/
/* -------------------------						*/
/* The following [enclosed] code is sample code	created	by IBM		*/
/* Corporation and Prominare Inc.  This	sample code is not part	of any	*/
/* standard IBM	product	and is provided	to you solely for the purpose	*/
/* of assisting	you in the development of your applications.  The code	*/
/* is provided "AS IS",	without	warranty of any	kind.  Neither IBM nor	*/
/* Prominare shall be liable for any damages arising out of your	*/
/* use of the sample code, even	if they	have been advised of the	*/
/* possibility of such damages.						*/

#define	INCL_DOS		   /* Include OS/2 DOS Kernal		*/
#define	INCL_GPI		   /* Include OS/2 PM GPI Interface	*/
#define	INCL_WIN		   /* Include OS/2 PM Windows Interface	*/

#include <os2.h>
#include <string.h>

#include "controls.h"

/* This	module contains	an example installable control that can	be used	*/
/* by any OS/2 2.x Presentation	Manager	application.  The control	*/
/* demonstrates	the principles of creating a custom control such	*/
/* that	new custom controls can	be created using this as a model.	*/
/*									*/
/* The control can be compiled using the IBM C Set/2 Version 1.0 with	*/
/* the following:							*/
/*									*/
/*   Icc -G3e- -O+ -Rn -W3 -C -FoControls.Obj Controls.C		*/
/*									*/
/* The control can be compiled using the WATCOM	C 386 Version 9.0 with	*/
/* the following:							*/
/*									*/
/*   Wcl386 -ms	-3s -bd	-s -ox -zp4 -w3	-c -fo=Controls.Obj Controls.C	*/

/* Filename:   Controls.C						*/

/*  Version:   1.00							*/
/*  Created:   1992-09-02						*/
/*  Revised:   1992-12-30						*/
/* Released:   1993-04-22						*/

/* Routines:   ULONG EXPENTRY InitControls(HAB hAB);			*/
/*	       static VOID CalcFrameSize(PRECTL	prcl, PSFRAME psframe);	*/
/*	       static VOID CalcLineSize(HWND hWnd, PRECTL prcl,		*/
/*					PLINEFIELD plf);		*/
/*	       static VOID CalcTextSize(PRECTL prcl, PTEXTFIELD	ptf);	*/
/*	       static LONG lGetPresParam(HWND hWnd, ULONG ulID1,	*/
/*					 ULONG ulID2, LONG lDefault);	*/
/*	       static VOID SetDefaultTextColours(HWND hWnd,		*/
/*						 PTEXTFIELD ptf);	*/
/*	       MRESULT EXPENTRY	FrameWndProc(HWND hWnd,	ULONG msg,	*/
/*					     MPARAM mp1, MPARAM	mp2);	*/
/*	       MRESULT EXPENTRY	Line3DWndProc(HWND hWnd, ULONG msg,	*/
/*					      MPARAM mp1, MPARAM mp2);	*/
/*	       MRESULT EXPENTRY	PatternWndProc(HWND hWnd, ULONG	msg,	*/
/*					       MPARAM mp1, MPARAM mp2);	*/
/*	       MRESULT EXPENTRY	Text3DWndProc(HWND hWnd, ULONG msg,	*/
/*					      MPARAM mp1, MPARAM mp2);	*/

/* Copyright ¸ International Business Machines Corp., 1991,1992,1993.	*/
/* Copyright ¸ 1989-1993  Prominare Inc.  All Rights Reserved.		*/

/* --------------------------------------------------------------------	*/


#define	USER_RESERVED	   16	   /* Control Reserved Memory Size	*/

#define	QUCWP_STYLE (QWL_USER +	 4)/* Pointer to Styles	Flag		*/
#define	QUCWP_HEAP  (QWL_USER +	 8)/* Pointer to Heap Handle		*/
#define	QUCWP_WNDP  (QWL_USER +	12)/* Pointer to Heap Pointer		*/


/************************************************************************/
/*									*/
/*     Private Control Data used by each different control type		*/
/*									*/
/************************************************************************/

typedef	struct _TEXTFIELD	   /* tf */
   {
   HWND	  hwndText;		   /* Static Text Handle		*/
   HWND	  hwndOwner;		   /* Owner Window Handle		*/
   POINTL aptl[5];		   /* Entry Field Outline Points	*/
   ULONG  lClrLeftTop;		   /* Left and Top Edge	Colour		*/
   ULONG  lClrBottomRight;	   /* Right and	Bottom Edge Colour	*/
   BOOL	  fRaised;		   /* Raised Field Flag			*/
   ULONG  aClr[7];		   /* Presentation Colours Array	*/
   } TEXTFIELD ;

typedef	TEXTFIELD *PTEXTFIELD;

typedef	struct _LINEFIELD	   /* lf */
   {
   HWND	  hwndOwner;		   /* Owner Window Handle		*/
   POINTL aptl[4];		   /* Entry Field Outline Points	*/
   } LINEFIELD ;

typedef	LINEFIELD *PLINEFIELD;

typedef	struct _SFRAME		   /* sframe */
   {
   POINTL aptl[2];		   /* Shadowed Frame Outline Points	*/
   POINTL aptlShadow[2];	   /* Shadowed Frame Outline Points	*/
   RECTL  rcl;			   /* Frame Rectangle			*/
   } SFRAME ;

typedef	SFRAME *PSFRAME;

typedef	struct _PATTERN		   /* pat */
   {
   RECTL    rcl;		   /* Frame Rectangle			*/
   HPOINTER hptrArrow;		   /* Arrow Pointer			*/
   ULONG    flStyle;		   /* Control Style			*/
   ULONG    aClr[7];		   /* Presentation Colours Array	*/
   } PATTERN ;

typedef	PATTERN	*PPATTERN;

/* --- Local Function Definitions -------------------------------------	*/

static VOID CalcFrameSize(PRECTL prcl, PSFRAME psframe);
static VOID CalcLineSize(HWND hWnd, PRECTL prcl, PLINEFIELD plf);
static VOID CalcTextSize(PRECTL	prcl, PTEXTFIELD ptf);
static LONG lGetPresParam(HWND hWnd, ULONG ulID1, ULONG	ulID2, LONG lDefault);
static VOID SetDefaultTextColours(HWND hWnd, PTEXTFIELD	ptf);

MRESULT	EXPENTRY FrameWndProc(HWND hWnd, ULONG msg, MPARAM mp1,	MPARAM mp2);
MRESULT	EXPENTRY Line3DWndProc(HWND hWnd, ULONG	msg, MPARAM mp1, MPARAM	mp2);
MRESULT	EXPENTRY PatternWndProc(HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT	EXPENTRY Text3DWndProc(HWND hWnd, ULONG	msg, MPARAM mp1, MPARAM	mp2);

/* --- Public Function Definitions ------------------------------------	*/

ULONG EXPENTRY InitControls(HAB	hAB);

// #pragma	subtitle("   Image Button Control - DLL Initialization/Termination Procedure")
// #pragma	page( )

/* --- InitControls ------------------------------------ [ Public ] ---	*/
/*									*/
/*     This function is	used to	provide	the control initialization	*/
/*     by registering the custom controls with OS/2 PM.			*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HAB hAB;	= Anchor Block Handle					*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     InitControls =  0 : Error Return					*/
/*		    =  1 : Successful Initialization			*/
/*									*/
/* --------------------------------------------------------------------	*/

ULONG EXPENTRY InitControls(HAB	hAB)

{
		       /* Register the control class with OS/2		*/
		       /* Presentation Manager and return registration	*/
		       /* result					*/

if ( !WinRegisterClass(hAB, (PCSZ) "3DFrame", (PFNWP)FrameWndProc,
		       CS_SYNCPAINT | CS_HITTEST | CS_PARENTCLIP | CS_SIZEREDRAW,
		       USER_RESERVED) )
   return(0UL);
		       /* Register the control class with OS/2		*/
		       /* Presentation Manager and return registration	*/
		       /* result					*/

if ( !WinRegisterClass(hAB, (PCSZ) "Patterns",	(PFNWP)PatternWndProc,
		       CS_SYNCPAINT | CS_HITTEST | CS_PARENTCLIP | CS_SIZEREDRAW,
		       USER_RESERVED) )
   return(0UL);
		       /* Register the control class with OS/2		*/
		       /* Presentation Manager and return registration	*/
		       /* result					*/

if ( !WinRegisterClass(hAB, (PCSZ) "3DLine", (PFNWP)Line3DWndProc,
		       CS_SYNCPAINT | CS_HITTEST | CS_PARENTCLIP | CS_SIZEREDRAW,
		       USER_RESERVED) )
   return(0UL);
		       /* Register the control class with OS/2		*/
		       /* Presentation Manager and return registration	*/
		       /* result					*/

if ( !WinRegisterClass(hAB, (PCSZ) "3DText", (PFNWP)Text3DWndProc,
		       CS_SYNCPAINT | CS_HITTEST | CS_PARENTCLIP | CS_SIZEREDRAW,
		       USER_RESERVED) )
   return(0UL);
		       /* Retrun successful registration value		*/
return(1UL);
}
// #pragma	subtitle("   Custom Controls - Window Size Calculation Procedure")
// #pragma	page ( )

/* --- CalcFrameSize ----------------------------------	[ Private ] -//--	*/
/*									*/
/*     This function is	used to	calculate the sizes and	positions	*/
/*     of the various elements that are	used to	make up	a shadowed	*/
/*     frame.								*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     PRECTL  prcl;	= Control Rectangle Pointer			*/
/*     PSFRAME psframe;	= Frame	Private	Data Pointer			*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     Nothing								*/
/*									*/
/* --------------------------------------------------------------------	*/

static VOID CalcFrameSize(PRECTL prcl, PSFRAME psframe)

{
	       /* Display points:					*/
	       /*							*/
	       /*	  1			      2			*/
	       /*	   +-------------------------+			*/
	       /*	   |			     |			*/
	       /*	   |			     |			*/
	       /*	   |			     |			*/
	       /*	   |			     |			*/
	       /*	   +-------------------------+			*/
	       /*	  0 4			      3			*/

psframe->rcl = *prcl;

psframe->aptl[0].x = prcl->xLeft + 1L;
psframe->aptl[0].y = prcl->yBottom;
psframe->aptl[1].x = prcl->xRight - 1L;
psframe->aptl[1].y = prcl->yTop	- 2L;

psframe->aptlShadow[0].x = prcl->xLeft;
psframe->aptlShadow[0].y = prcl->yBottom + 1L;
psframe->aptlShadow[1].x = prcl->xRight	- 2L;
psframe->aptlShadow[1].y = prcl->yTop -	1L;
}
// #pragma	subtitle("   Custom Controls - Control Window Procedure")
// #pragma	page ( )

/* --- CalcLineSize -----------------------------------	[ Private ] ---	*/
/*									*/
/*     This function is	used to	calculate the sizes and	positions	*/
/*     of the various elements that are	used to	make up	a 3D line	*/
/*     field.								*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HWND	  hWnd;	= Window Handle					*/
/*     PRECTL	  prcl;	= Control Rectangle Pointer			*/
/*     PLINEFIELD ptf;	= Private Line Data Pointer			*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     Nothing								*/
/*									*/
/* --------------------------------------------------------------------	*/

static VOID CalcLineSize(HWND hWnd, PRECTL prcl, PLINEFIELD plf)

{
	       /*  Line	display	points					*/
	       /*							*/
	       /*	  0			      1			*/
	       /*	   ===========================			*/
	       /*	  2			      3			*/

if ( WinQueryWindowULong(hWnd, QWL_STYLE) & LNS_VERTICAL )
   {
   plf->aptl[0].x = plf->aptl[1].x = (prcl->xRight - prcl->xLeft) / 2L +
				      prcl->xLeft;
   plf->aptl[2].x = plf->aptl[3].x = plf->aptl[0].x + 1L;
   plf->aptl[0].y = plf->aptl[2].y = prcl->yBottom;
   plf->aptl[1].y = plf->aptl[3].y = prcl->yTop	  - 1L;
   }
else
   {
   plf->aptl[0].x = plf->aptl[2].x = prcl->xLeft;
   plf->aptl[1].x = plf->aptl[3].x = prcl->xRight;
   plf->aptl[0].y = plf->aptl[1].y = (prcl->yTop - prcl->yBottom) / 2L +
				      prcl->yBottom;
   plf->aptl[2].y = plf->aptl[3].y = plf->aptl[0].y - 1L;
   }
}
// #pragma	subtitle("   Custom Controls - Control Window Sizing Procedure")
// #pragma	page ( )

/* --- CalcTextSize -----------------------------------	[ Private ] ---	*/
/*									*/
/*     This function is	used to	calculate the sizes and	positions	*/
/*     of the various elements that are	used to	make up	a shadowed	*/
/*     text field.							*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     PRECTL	  prcl;	= Control Rectangle Pointer			*/
/*     PTEXTFIELD ptf;	= Private Text Data Pointer			*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     Nothing								*/
/*									*/
/* --------------------------------------------------------------------	*/

static VOID CalcTextSize(PRECTL	prcl, PTEXTFIELD ptf)

{
	       /*  Text	display	points					*/
	       /*							*/
	       /*	  1			      2			*/
	       /*	   +-------------------------+			*/
	       /*	   |			     |			*/
	       /*	   |			     |			*/
	       /*	   |			     |			*/
	       /*	   |			     |			*/
	       /*	   +-------------------------+			*/
	       /*	  0 4			      3			*/

ptf->aptl[0].x = prcl->xLeft;
ptf->aptl[0].y = prcl->yBottom;
ptf->aptl[1].x = prcl->xLeft;
ptf->aptl[1].y = prcl->yTop   -	1L;
ptf->aptl[2].x = prcl->xRight -	1L;
ptf->aptl[2].y = prcl->yTop   -	1L;
ptf->aptl[3].x = prcl->xRight -	1L;
ptf->aptl[3].y = prcl->yBottom;
ptf->aptl[4].x = prcl->xLeft;
ptf->aptl[4].y = prcl->yBottom;

WinSetWindowPos(ptf->hwndText, HWND_BOTTOM,
		(prcl->xLeft + 2L), (prcl->yBottom + 1L),
		(prcl->xRight -	prcl->xLeft - 4L),
		(prcl->yTop - prcl->yBottom - 2L),
		SWP_MOVE | SWP_SIZE | SWP_SHOW);
}
// #pragma	subtitle("   Custom Controls - Control Window Procedure")
// #pragma	page( )

/* --- lGetPresParam ----------------------------------	[ Private } ---	*/
/*									*/
/*     This function is	used to	retrieve a presentation	parameter	*/
/*     that may	be present.  If	the presentation parameter is not,	*/
/*     the default colour passed to the	function will be used.		*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HWND  hWnd;     = Window	Handle					*/
/*     ULONG ulID1;    = Presentation Parameter	1 ID			*/
/*     ULONG ulID2;    = Presentation Parameter	2 ID			*/
/*     LONG  lDefault; = Default Colour					*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     lGetPresParam = Colour to Use					*/
/*									*/
/* --------------------------------------------------------------------	*/

static LONG lGetPresParam(HWND hWnd, ULONG ulID1, ULONG	ulID2, LONG lDefault)

{
HPS   hPS;			   /* Presentation Space Handle		*/
LONG  lClr;			   /* Presentation Parameter Colour	*/
ULONG ulID;			   /* Presentation Parameter ID		*/

if ( WinQueryPresParam(hWnd, ulID1, ulID2, &ulID, 4, (PVOID)&lClr,
		       QPF_NOINHERIT | QPF_ID1COLORINDEX | QPF_ID2COLORINDEX | QPF_PURERGBCOLOR) )
   return(lClr);
else
   if (	(lDefault >= SYSCLR_SHADOWHILITEBGND) &&
	(lDefault <= SYSCLR_HELPHILITE)	)
       return(WinQuerySysColor(HWND_DESKTOP, lDefault, 0L));
   else
       if ( (lClr = GpiQueryRGBColor(hPS = WinGetPS(hWnd),
				     LCOLOPT_REALIZED, lDefault)) == GPI_ALTERROR )
	   {
	   WinReleasePS(hPS);
	   return(lDefault);
	   }
       else
	   {
	   WinReleasePS(hPS);
	   return(lClr);
	   }
}
// #pragma	subtitle("   Custom Controls - Default Colours Procedure")
// #pragma	page( )

/* --- SetDefaultTextColours --------------------------	[ Private ] ---	*/
/*									*/
/*     This function is	used to	set the	default	colours	that the	*/
/*     image button should use within the internal paint routines.	*/
/*     The colour can either be	a presentation parameter that has	*/
/*     been set	or it can be the default colour	as defined within	*/
/*     control.								*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HWND	  hWnd;	= Window Handle					*/
/*     PTEXTFIELD ptf;	= Text Control Structure Pointer		*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     Nothing								*/
/*									*/
/* --------------------------------------------------------------------	*/

static VOID SetDefaultTextColours(HWND hWnd, PTEXTFIELD	ptf)

{
		       /* Set up the colours that will be used within	*/
		       /* the painting of the control.	The colour	*/
		       /* indices are:					*/
		       /*						*/
		       /* 0 : Foreground (PP_FOREGROUND*)		*/
		       /* 1 : Background (PP_BACKGROUND*)		*/
		       /* 2 : Hilight Foreground (PP_HILITEFOREGROUND*)	*/
		       /* 3 : Hilight Background (PP_HILITEBACKGROUND*)	*/
		       /* 4 : Disabled Foreground (PP_DISABLEDFORE*)	*/
		       /* 5 : Disabled Foreground (PP_DISABLEDFORE*)	*/
		       /* 6 : Border (PP_BORDER*)			*/

ptf->aClr[0] = lGetPresParam(hWnd, PP_FOREGROUNDCOLOR,
			     PP_FOREGROUNDCOLORINDEX,
			     SYSCLR_OUTPUTTEXT);
ptf->aClr[1] = lGetPresParam(hWnd, PP_BACKGROUNDCOLOR,
			     PP_BACKGROUNDCOLORINDEX,
			     SYSCLR_FIELDBACKGROUND);
ptf->aClr[2] = lGetPresParam(hWnd, PP_HILITEFOREGROUNDCOLOR,
			     PP_HILITEFOREGROUNDCOLORINDEX,
			     SYSCLR_OUTPUTTEXT);
ptf->aClr[3] = lGetPresParam(hWnd, PP_HILITEBACKGROUNDCOLOR,
			     PP_HILITEBACKGROUNDCOLORINDEX,
			     SYSCLR_BACKGROUND);
ptf->aClr[4] = lGetPresParam(hWnd, PP_DISABLEDFOREGROUNDCOLOR,
			     PP_DISABLEDFOREGROUNDCOLORINDEX,
			     SYSCLR_OUTPUTTEXT);
ptf->aClr[5] = lGetPresParam(hWnd, PP_DISABLEDBACKGROUNDCOLOR,
			     PP_DISABLEDBACKGROUNDCOLORINDEX,
			     SYSCLR_BACKGROUND);
ptf->aClr[6] = lGetPresParam(hWnd, PP_BORDERCOLOR,
			     PP_BORDERCOLORINDEX,
			     SYSCLR_BUTTONDARK);
}
// #pragma	subtitle("   Custom Controls - Control Window Procedure")
// #pragma	page ( )

/* --- FrameWndProc ---------------------------------------------------	*/
/*									*/
/*     This function is	used to	handle the messages sent to the		*/
/*     installed control.  The window procedure	is designed to		*/
/*     allow for multiple instances and	to be totally re-entrant.	*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HWND   hWnd; = Window Handle					*/
/*     ULONG  msg;  = PM Message					*/
/*     MPARAM mp1;  = Message Parameter	1				*/
/*     MPARAM mp2;  = Message Parameter	2				*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     FrameWndProc = Message Handling Result				*/
/*									*/
/* --------------------------------------------------------------------	*/

MRESULT	EXPENTRY FrameWndProc(HWND hWnd, ULONG msg, MPARAM mp1,	MPARAM mp2)

{
HPS	      hPS;		   /* Presentation Space Handle		*/
PCREATESTRUCT pcrst;		   /* Create Structure Pointer		*/
PSFRAME	      psframe;		   /* Frame Private Data Pointer	*/
PWNDPARAMS    pwprm;		   /* Window Parameters	Pointer		*/
RECTL	      rcl;		   /* Rectangle	Holder			*/

switch ( msg )
   {

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	1: Control creation coding					*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Control creation							*/
   /*********************************************************************/

   case	WM_CREATE :
		       /* Allocate memory for the private control data	*/

       DosAllocMem((PPVOID)&psframe, 4096UL, PAG_READ |	PAG_WRITE | PAG_COMMIT);

		       /* Save the private control data	pointer	within	*/
		       /* the controls reserved	memory			*/

       WinSetWindowPtr(hWnd, QUCWP_WNDP, (PVOID)psframe);

		       /* Get the control's creation structure address  */
		       /* to save the size of the control		*/

       pcrst = (PCREATESTRUCT)PVOIDFROMMP(mp2);
       psframe->rcl.xLeft  = psframe->rcl.yBottom = 0L;
       psframe->rcl.xRight = pcrst->cx;
       psframe->rcl.yTop   = pcrst->cy;

		       /* Determine the	display	points for the shadow	*/

       CalcFrameSize(&psframe->rcl, psframe);
       break;

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	2: Control text	and colour support coding			*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Process window parameters query					*/
   /*********************************************************************/

   case	WM_QUERYWINDOWPARAMS :

		       /* Get the address for the windows parameters	*/
		       /* structure					*/

       pwprm = (PWNDPARAMS)PVOIDFROMMP(mp1);

		       /* Determine the	type of	query			*/

       switch (	pwprm->fsStatus	)
	   {
		       /* Query	type:  get text	length			*/

	   case	WPM_CCHTEXT :

		       /* Place	the length the string within the	*/
		       /* structure					*/

	       pwprm->cchText =	0;
	       break;
		       /* Query	type:  get control data	length		*/

	   case	WPM_CBCTLDATA :

		       /* Set the control data length to zero		*/

	       pwprm->cbCtlData	= 0;
	       break;

	   default :
	       return(WinDefWindowProc(hWnd, msg, mp1, mp2));
	   }
       break;

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	3: Control size	and position coding				*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Size of the control changing					*/
   /*********************************************************************/

   case	WM_SIZE	:
       WinQueryWindowRect(hWnd,	&rcl);
       CalcFrameSize(&rcl,
		     (PSFRAME)WinQueryWindowPtr(hWnd, QUCWP_WNDP));
       break;

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	4: User	interaction coding for keyboard	and mouse		*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Process mouse hit testing					*/
   /*********************************************************************/

   case	WM_HITTEST :
       return(MRFROMLONG(HT_TRANSPARENT));

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	5: Control painting and	appearance coding			*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Erase background							*/
   /*********************************************************************/

   case	WM_ERASEBACKGROUND :

		       /* Have OS/2 Presentation Manager perform the	*/
		       /* background erase on behalf of	the button	*/

       return(MRFROMLONG(TRUE));

   /*********************************************************************/
   /*  Paint the button							*/
   /*********************************************************************/

   case	WM_PAINT :
		       /* Get the address of the private data from the	*/
		       /* control's reserved memory                     */

       psframe = (PSFRAME)WinQueryWindowPtr(hWnd, QUCWP_WNDP);

		       /* Get the presentation space for the control	*/
		       /* and set the colour table to RGB mode		*/

       GpiCreateLogColorTable(hPS = WinBeginPaint(hWnd,	(HPS)NULL, (PRECTL)NULL),
			      0L, LCOLF_RGB, 0L, 0L, (PLONG)NULL);

		       /* Set the colour of the	highlight edge to white	*/

       GpiSetColor(hPS,	0x00FFFFFF);

		       /* Draw the highlighted part of the frame	*/

       GpiMove(hPS, psframe->aptl);
       GpiBox(hPS, DRO_OUTLINE,	&psframe->aptl[1], 0L, 0L);

		       /* Set the colour for the shadow	component	*/

       GpiSetColor(hPS,	(LONG)WinQuerySysColor(HWND_DESKTOP, SYSCLR_SHADOW, 0L));

		       /* Draw the shadow part of the frame		*/

       GpiMove(hPS, psframe->aptlShadow);
       GpiBox(hPS, DRO_OUTLINE,	&psframe->aptlShadow[1], 0L, 0L);

       WinEndPaint(hPS);
       break;

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	6: Control destruction coding					*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Window being destroyed, perform clean-up				*/
   /*********************************************************************/

   case	WM_DESTROY :
		       /* Get the address of the private data from the	*/
		       /* control's reserved memory and release the     */
		       /* allocated memory				*/

       DosFreeMem((PVOID)WinQueryWindowPtr(hWnd, QUCWP_WNDP));
       break;
		       /* Default message processing			*/
   default :
       return(WinDefWindowProc(hWnd, msg, mp1, mp2));
   }

return(0L);
}
// #pragma	subtitle("   Custom Controls - Control Window Procedure")
// #pragma	page ( )

/* --- Line3DWndProc --------------------------------------------------	*/
/*									*/
/*     This function is	used to	handle the messages sent to the		*/
/*     installed control.  The window procedure	is designed to		*/
/*     allow for multiple instances and	to be totally re-entrant.	*/
/*									*/
/*     The styles supported for	the control are:			*/
/*									*/
/*     LNS_HORZ						*/
/*     Upon Entry:							*/
/*     HWND   hWnd; = Window Handle					*/
/*     ULONG  msg;  = PM Message					*/
/*     MPARAM mp1;  = Message Parameter	1				*/
/*     MPARAM mp2;  = Message Parameter	2				*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     Line3DWndProc = Message Handling	Result				*/
/*									*/
/* --------------------------------------------------------------------	*/

MRESULT	EXPENTRY Line3DWndProc(HWND hWnd, ULONG	msg, MPARAM mp1, MPARAM	mp2)

{
HPS	      hPS;		   /* Presentation Space Handle		*/
PCREATESTRUCT pcrst;		   /* Create Structure Pointer		*/
PLINEFIELD    plf;		   /* Text Field Structure Pointer	*/
RECTL	      rcl;		   /* Rectangle	Holder			*/
PWNDPARAMS    pwprm;		   /* Window Parameters	Pointer		*/

switch ( msg )
   {

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	1: Control creation coding					*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Control creation							*/
   /*********************************************************************/

   case	WM_CREATE :
		       /* Allocate memory for the private control data	*/

       DosAllocMem((PPVOID)&plf, 4096UL, PAG_COMMIT | PAG_READ | PAG_WRITE);

		       /* Save the private control data	pointer	within	*/
		       /* the controls reserved	memory			*/

       WinSetWindowPtr(hWnd, QUCWP_WNDP, (PVOID)plf);

		       /* Get the control's creation structure address  */
		       /* to save the size of the control  and the	*/
		       /* control owner					*/

       pcrst = (PCREATESTRUCT)PVOIDFROMMP(mp2);

       plf->hwndOwner =	pcrst->hwndOwner;

       rcl.xLeft   =\
       rcl.yBottom = 0L;
       rcl.xRight  = pcrst->cx;
       rcl.yTop	   = pcrst->cy;
       CalcLineSize(hWnd, &rcl,	plf);
       break;

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	2: Control text	and colour support coding			*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Process window parameters query					*/
   /*********************************************************************/

   case	WM_QUERYWINDOWPARAMS :

		       /* Get the address for the windows parameters	*/
		       /* structure					*/

       pwprm = (PWNDPARAMS)PVOIDFROMMP(mp1);

		       /* Determine the	type of	query			*/

       switch (	pwprm->fsStatus	)
	   {
		       /* Query	type:  get text	length			*/

	   case	WPM_CCHTEXT :

		       /* Place	the length the string within the	*/
		       /* structure					*/

	       pwprm->cchText =	0;
	       break;
		       /* Query	type:  get control data	length		*/

	   case	WPM_CBCTLDATA :

		       /* Set the control data length to zero		*/

	       pwprm->cbCtlData	= 0;
	       break;

	   default :
	       return(WinDefWindowProc(hWnd, msg, mp1, mp2));
	   }
       break;

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	3: Control size	and position coding				*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Size of the control changing					*/
   /*********************************************************************/

   case	WM_SIZE	:
       WinQueryWindowRect(hWnd,	&rcl);
       CalcLineSize(hWnd, &rcl,
		plf = (PLINEFIELD)WinQueryWindowPtr(hWnd, QUCWP_WNDP));
       break;

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	4: User	interaction coding for keyboard	and mouse		*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Process mouse hit testing					*/
   /*********************************************************************/

   case	WM_HITTEST :
       return(MRFROMLONG(HT_TRANSPARENT));

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	5: Control painting and	appearance coding			*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Erase background							*/
   /*********************************************************************/

   case	WM_ERASEBACKGROUND :

		       /* Have OS/2 Presentation Manager perform the	*/
		       /* background erase on behalf of	the button	*/

       return(MRFROMLONG(TRUE));

   /*********************************************************************/
   /*  Paint the button							*/
   /*********************************************************************/

   case	WM_PAINT :
		       /* Get the address of the private data from the	*/
		       /* control's reserved memory                     */

       plf = (PLINEFIELD)WinQueryWindowPtr(hWnd, QUCWP_WNDP);

		       /* Get the presentation space for the control	*/
		       /* and set the colour table to RGB mode		*/

       GpiCreateLogColorTable(hPS = WinBeginPaint(hWnd,	(HPS)NULL, (PRECTL)NULL),
			      0L, LCOLF_RGB, 0L, 0L, (PLONG)NULL);

		       /* Set the shadow colour	for the	line to	dark	*/
		       /* gray						*/

       GpiSetColor(hPS,	(LONG)WinQuerySysColor(HWND_DESKTOP, SYSCLR_SHADOW, 0L));

		       /* Display the shadow portion			*/

       GpiMove(hPS, plf->aptl);
       GpiLine(hPS, &plf->aptl[1]);

		       /* Set the highlight colour for the line	to	*/
		       /* white						*/

       GpiSetColor(hPS,	0x00ffffff);

		       /* Display the highlight	portion			*/

       GpiMove(hPS, &plf->aptl[2]);
       GpiLine(hPS, &plf->aptl[3]);

       WinEndPaint(hPS);
       break;

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	6: Control destruction coding					*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Window being destroyed, perform clean-up				*/
   /*********************************************************************/

   case	WM_DESTROY :
		       /* Get the address of the private data from the	*/
		       /* control's reserved memory and release the     */
		       /* memory allocated for the use of the control	*/

       DosFreeMem((PVOID)WinQueryWindowPtr(hWnd, QUCWP_WNDP));
       break;
		       /* Default message processing			*/
   default :
       return(WinDefWindowProc(hWnd, msg, mp1, mp2));
   }

return(0L);
}
// #pragma	subtitle("   Custom Controls - Control Window Procedure")
// #pragma	page( )

/* --- PatternWndProc -------------------------------------------------	*/
/*									*/
/*     This function is	used to	process	the messages for the image	*/
/*     button control.							*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HWND   hWnd; = Window Handle					*/
/*     ULONG  msg;  = PM Message					*/
/*     MPARAM mp1;  = Message Parameter	1				*/
/*     MPARAM mp2;  = Message Parameter	2				*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     PatternWndProc =	Message	Handling Result				*/
/*									*/
/* --------------------------------------------------------------------	*/

MRESULT	EXPENTRY PatternWndProc(HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2)

{
HPS	      hPS;		   /* Presentation Space Handle		*/
PCREATESTRUCT pcrst;		   /* Create Structure Pointer		*/
POINTL	      ptl;		   /* Drawing Point			*/
PPATTERN      ppat;		   /* Image Button Structure Pointer	*/
PWNDPARAMS    pwprm;		   /* Window Parameters	Pointer		*/

switch ( msg )
   {

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	1: Control creation coding					*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Control creation							*/
   /*********************************************************************/

   case	WM_CREATE :
		       /* Allocate memory for the private control data	*/

       DosAllocMem((PVOID)&ppat, 4096UL, PAG_READ | PAG_WRITE |	PAG_COMMIT);

		       /* Save the private control data	pointer	within	*/
		       /* the controls reserved	memory			*/

       WinSetWindowPtr(hWnd, QUCWP_WNDP, (PVOID)ppat);

		       /* Get the control's creation structure address  */
		       /* to save the size of the control		*/

       pcrst = (PCREATESTRUCT)PVOIDFROMMP(mp2);

       ppat->rcl.xLeft	= ppat->rcl.yBottom = 0L;
       ppat->rcl.xRight	= pcrst->cx;
       ppat->rcl.yTop	= pcrst->cy;
       break;

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	2: Control text	and colour support coding			*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Process window parameters query					*/
   /*********************************************************************/

   case	WM_QUERYWINDOWPARAMS :

		       /* Get the address for the windows parameters	*/
		       /* structure					*/

       pwprm = (PWNDPARAMS)PVOIDFROMMP(mp1);

		       /* Determine the	type of	query			*/

       switch (	pwprm->fsStatus	)
	   {
		       /* Query	type:  get text	length			*/

	   case	WPM_CCHTEXT :

		       /* Place	the length the string within the	*/
		       /* structure					*/

	       pwprm->cchText =	0;
	       break;
		       /* Query	type:  get control data	length		*/

	   case	WPM_CBCTLDATA :

		       /* Set the control data length to zero		*/

	       pwprm->cbCtlData	= 0;
	       break;

	   default :
	       return(WinDefWindowProc(hWnd, msg, mp1, mp2));
	   }
       break;

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	3: Control size	and position coding				*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Size of the control changing					*/
   /*********************************************************************/

   case	WM_SIZE	:
       ppat = (PPATTERN)WinQueryWindowPtr(hWnd,	QUCWP_WNDP);
       WinQueryWindowRect(hWnd,	&ppat->rcl);
       break;

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	4: User	interaction coding for keyboard	and mouse		*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Process mouse hit testing					*/
   /*********************************************************************/

   case	WM_HITTEST :
       return(MRFROMLONG(HT_TRANSPARENT));

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	5: Control painting and	appearance coding			*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Erase background							*/
   /*********************************************************************/

   case	WM_ERASEBACKGROUND :

		       /* Have OS/2 Presentation Manager perform the	*/
		       /* background erase on behalf of	the button	*/

       return(MRFROMLONG(TRUE));

   /*********************************************************************/
   /*  Paint the button							*/
   /*********************************************************************/

   case	WM_PAINT :
		       /* Get the address of the private data from the	*/
		       /* control's reserved memory                     */

       ppat = (PPATTERN)WinQueryWindowPtr(hWnd,	QUCWP_WNDP);

		       /* Get the presentation space for the window	*/
		       /* and draw the grid which the buttons are	*/
		       /* placed					*/

       GpiSetPattern(hPS = WinBeginPaint(hWnd, (HPS)NULL, (PRECTL)NULL),
		     (LONG)WinQueryWindowULong(hWnd, QWL_STYLE)	& 0x5f);

       ptl.x = ptl.y = 0L;
       GpiMove(hPS, &ptl);
       ptl.x = ppat->rcl.xRight;
       ptl.y = ppat->rcl.yTop;
       GpiBox(hPS, DRO_FILL, &ptl, 0L, 0L);

		       /* Release the presentation space		*/
       WinEndPaint(hPS);
       break;

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	6: Control destruction coding					*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Window being destroyed, perform clean-up				*/
   /*********************************************************************/

   case	WM_DESTROY :
		       /* Release the heap allocated for use by	the	*/
		       /* control					*/

       DosFreeMem((PVOID)WinQueryWindowPtr(hWnd, QUCWP_WNDP));
       break;
		       /* Default message processing			*/
   default :
       return(WinDefWindowProc(hWnd, msg, mp1, mp2));
   }
return(0L);
}
// #pragma	subtitle("   Custom Controls - 3D Text Window Procedure")
// #pragma	page ( )

/* --- Text3DWndProc ----------------------------------	[ Private ] ---	*/
/*									*/
/*     This function is	used to	handle the messages sent to the		*/
/*     installed control.  The window procedure	is designed to		*/
/*     allow for multiple instances and	to be totally re-entrant.	*/
/*									*/
/*     Upon Entry:							*/
/*									*/
/*     HWND   hWnd; = Window Handle					*/
/*     ULONG  msg;  = PM Message					*/
/*     MPARAM mp1;  = Message Parameter	1				*/
/*     MPARAM mp2;  = Message Parameter	2				*/
/*									*/
/*     Upon Exit:							*/
/*									*/
/*     Text3DWndProc = Message Handling	Result				*/
/*									*/
/* --------------------------------------------------------------------	*/

MRESULT	EXPENTRY Text3DWndProc(HWND hWnd, ULONG	msg, MPARAM mp1, MPARAM	mp2)

{
HPS	       hPS;		   /* Presentation Space Handle		*/
LONG	       lClr;		   /* Presentation Parameter Colour	*/
PCREATESTRUCT  pcrst;		   /* Create Structure Pointer		*/
PTEXTFIELD     ptf;		   /* Text Field Structure Pointer	*/
RECTL	       rcl;		   /* Rectangle	Holder			*/
ULONG	       ulID;		   /* Presentation Parameter ID		*/

switch ( msg )
   {

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	1: Control creation coding					*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Control creation							*/
   /*********************************************************************/

   case	WM_CREATE :
		       /* Allocate memory for the private control data	*/

       DosAllocMem((PVOID)&ptf,	4096UL,	PAG_READ | PAG_WRITE | PAG_COMMIT);

		       /* Save the private control data	pointer	within	*/
		       /* the controls reserved	memory			*/

       WinSetWindowPtr(hWnd, QUCWP_WNDP, (PVOID)ptf);

		       /* Get the control's creation structure address  */
		       /* to save the size of the control		*/

       pcrst = (PCREATESTRUCT)PVOIDFROMMP(mp2);

       ptf->hwndOwner =	pcrst->hwndOwner;
       ptf->hwndText  =	WinCreateWindow(hWnd, WC_STATIC, pcrst->pszText,
					(pcrst->flStyle	& ~DS_RAISED) |	WS_VISIBLE | SS_TEXT,
					0, 0, 0, 0,
					hWnd, HWND_BOTTOM, 0x4321,
					pcrst->pCtlData, pcrst->pPresParams);

		       /* Set up the colours that will be used within	*/
		       /* the painting of the control.	The colour	*/
		       /* indices are:					*/
		       /*						*/
		       /* 0 : Foreground (PP_FOREGROUND*)		*/
		       /* 1 : Background (PP_BACKGROUND*)		*/
		       /* 2 : Hilight Foreground (PP_HILITEFOREGROUND*)	*/
		       /* 3 : Hilight Background (PP_HILITEBACKGROUND*)	*/
		       /* 4 : Disabled Foreground (PP_DISABLEDFORE*)	*/
		       /* 5 : Disabled Foreground (PP_DISABLEDFORE*)	*/
		       /* 6 : Border (PP_BORDER*)			*/

       SetDefaultTextColours(hWnd, ptf);
       WinSetPresParam(ptf->hwndText, PP_FOREGROUNDCOLOR, 4L, (PVOID)ptf->aClr);
       WinSetPresParam(ptf->hwndText, PP_BACKGROUNDCOLOR, 4L, (PVOID)&ptf->aClr[1]);

		       /* Check	to see if the user provided font that	*/
		       /* should override the default font that	would	*/
		       /* be set					*/

       if ( !WinQueryPresParam(hWnd, PP_FONTNAMESIZE, 0L, &ulID, 4L,
			       (PVOID)&lClr, QPF_NOINHERIT) )

		       /* System indicates not set since not data was	*/
		       /* returned, therefore set default font for the	*/
		       /* control					*/

	   WinSetPresParam(hWnd, PP_FONTNAMESIZE, 8L, (PVOID)"10.Helv");

       if ( pcrst->flStyle & DS_RAISED )
	   {
	   ptf->fRaised	= TRUE;
	   ptf->lClrLeftTop	= 0x00ffffff;
	   ptf->lClrBottomRight	= ptf->aClr[6];
	   }
       else
	   {
	   ptf->fRaised	= FALSE;
	   ptf->lClrLeftTop	= ptf->aClr[6];
	   ptf->lClrBottomRight	= 0x00ffffff;
	   }

       rcl.xLeft   =\
       rcl.yBottom = 0L;
       rcl.xRight  = pcrst->cx;
       rcl.yTop	   = pcrst->cy;
       CalcTextSize(&rcl, ptf);
       break;

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	2: Control text	and colour support coding			*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Process window parameters query					*/
   /*********************************************************************/

   case	WM_QUERYWINDOWPARAMS :
   case	WM_SETWINDOWPARAMS :
       ptf = (PTEXTFIELD)WinQueryWindowPtr(hWnd, QUCWP_WNDP);
       return(MRFROMLONG(WinSendMsg(ptf->hwndText, msg,	mp1, mp2)));

   /*********************************************************************/
   /*  Process window presentation parameters change			*/
   /*********************************************************************/

   case	WM_PRESPARAMCHANGED :
       if ( LONGFROMMP(mp1) < PP_FONTNAMESIZE )
	   {
	   ptf = (PTEXTFIELD)WinQueryWindowPtr(hWnd, QUCWP_WNDP);
	   if (	WinQueryPresParam(hWnd,	LONGFROMMP(mp1),
				  (LONGFROMMP(mp1) % 2)	? LONGFROMMP(mp1) + 1L : 0L,
				  &ulID, 4, (PVOID)&lClr,
				  QPF_NOINHERIT	| QPF_ID1COLORINDEX | QPF_ID2COLORINDEX) )
	       ptf->aClr[(LONGFROMMP(mp1) - 1L)	/ 2L] =	lClr;
	   if (	ptf->fRaised )
	       ptf->lClrBottomRight = ptf->aClr[6];
	   else
	       ptf->lClrLeftTop	    = ptf->aClr[6];
	   WinInvalidateRect(hWnd, (PRECTL)NULL, TRUE);
	   }
		       /* Check	to see if an individual	presentation	*/
		       /* parameter has	changed	if so, get the new	*/
		       /* colour value for use by the painting routines	*/

       if ( LONGFROMMP(mp1) && (LONGFROMMP(mp1)	< PP_FONTNAMESIZE) )
	   {
		       /* Get the address of the private data from the	*/
		       /* control's reserved memory                     */

	   ptf = (PTEXTFIELD)WinQueryWindowPtr(hWnd, QUCWP_WNDP);

		       /* Get the new presentation parameter colour for	*/
		       /* the presentation parameter that has changed.	*/
		       /* Get the colour as a RGB value	so as to be	*/
		       /* able to get an exact value and not an		*/
		       /* approximation	which could happen if the	*/
		       /* presentation parameter was set as a RGB but	*/
		       /* queried as an	index.	When WinQueryPresParam	*/
		       /* returns a 0, it indicates that no		*/
		       /* presentation parameter set and the default	*/
		       /* colours should be used.			*/

	   switch ( LONGFROMMP(mp1) )
	       {
	       case PP_FOREGROUNDCOLOR :
	       case PP_FOREGROUNDCOLORINDEX :
		   ptf->aClr[0]	= lGetPresParam(hWnd, PP_FOREGROUNDCOLOR,
						PP_FOREGROUNDCOLORINDEX,
						SYSCLR_OUTPUTTEXT);
		   WinSetPresParam(ptf->hwndText,
				   PP_FOREGROUNDCOLOR,
				   4L, (PVOID)&ptf->aClr[0]);
		   break;

	       case PP_BACKGROUNDCOLOR :
	       case PP_BACKGROUNDCOLORINDEX :
		   ptf->aClr[1]	= lGetPresParam(hWnd, PP_BACKGROUNDCOLOR,
						PP_BACKGROUNDCOLORINDEX,
						SYSCLR_BACKGROUND);
		   WinSetPresParam(ptf->hwndText,
				   PP_BACKGROUNDCOLOR,
				   4L, (PVOID)&ptf->aClr[1]);
		   break;

	       case PP_HILITEFOREGROUNDCOLOR :
	       case PP_HILITEFOREGROUNDCOLORINDEX :
		   ptf->aClr[2]	= lGetPresParam(hWnd, PP_HILITEFOREGROUNDCOLOR,
						PP_HILITEFOREGROUNDCOLORINDEX,
						SYSCLR_OUTPUTTEXT);
		   break;

	       case PP_HILITEBACKGROUNDCOLOR :
	       case PP_HILITEBACKGROUNDCOLORINDEX :
		   ptf->aClr[3]	= lGetPresParam(hWnd, PP_HILITEBACKGROUNDCOLOR,
						PP_HILITEBACKGROUNDCOLORINDEX,
						SYSCLR_BACKGROUND);
		   break;

	       case PP_DISABLEDFOREGROUNDCOLOR :
	       case PP_DISABLEDFOREGROUNDCOLORINDEX :
		   ptf->aClr[4]	= lGetPresParam(hWnd, PP_DISABLEDFOREGROUNDCOLOR,
						PP_DISABLEDFOREGROUNDCOLORINDEX,
						SYSCLR_OUTPUTTEXT);
		   break;

	       case PP_DISABLEDBACKGROUNDCOLOR :
	       case PP_DISABLEDBACKGROUNDCOLORINDEX :
		   ptf->aClr[5]	= lGetPresParam(hWnd, PP_BACKGROUNDCOLOR,
						PP_BACKGROUNDCOLORINDEX,
						SYSCLR_BACKGROUND);
		   break;

	       case PP_BORDERCOLOR :
	       case PP_BORDERCOLORINDEX	:
		   ptf->aClr[6]	= lGetPresParam(hWnd, PP_BORDERCOLOR,
						PP_BORDERCOLORINDEX,
						SYSCLR_BUTTONDARK);
		   break;
	       }
	   if (	ptf->fRaised )
	       ptf->lClrBottomRight = ptf->aClr[6];
	   else
	       ptf->lClrLeftTop	    = ptf->aClr[6];

		       /* Invalidate the button	to force to use	the	*/
		       /* new colours just set or removed		*/

	   WinInvalidateRect(hWnd, (PRECTL)NULL, TRUE);
	   }
       else
		       /* Determine if the Scheme Palette has forced a	*/
		       /* global scheme	update in which	case, check all	*/
		       /* of the presentation parameters to see	if they	*/
		       /* have been added or removed			*/

	   if (	LONGFROMMP(mp1)	== 0L )

		       /* Set up the colours that will be used within	*/
		       /* the painting of the control.			*/

	       SetDefaultTextColours(hWnd,
				 (PTEXTFIELD)WinQueryWindowPtr(hWnd, QUCWP_WNDP));
       break;

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	3: Control size	and position coding				*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Size of the control changing					*/
   /*********************************************************************/

   case	WM_SIZE	:
       WinQueryWindowRect(hWnd,	&rcl);
       CalcTextSize(&rcl,
		    (PTEXTFIELD)WinQueryWindowPtr(hWnd,	QUCWP_WNDP));
       break;

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	4: User	interaction coding for keyboard	and mouse		*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Process control notification					*/
   /*********************************************************************/

   case	WM_CONTROL :
			/* Since the control is	really a composite,	*/
			/* pass	the message to the text	control		*/

       ptf = (PTEXTFIELD)WinQueryWindowPtr(hWnd, QUCWP_WNDP);
       return(MRFROMLONG(WinSendMsg(ptf->hwndOwner, msg, mp1, mp2)));

   /*********************************************************************/
   /*  Process mouse hit testing					*/
   /*********************************************************************/

   case	WM_HITTEST :
       return(MRFROMLONG(HT_TRANSPARENT));

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	5: Control painting and	appearance coding			*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Erase background							*/
   /*********************************************************************/

   case	WM_ERASEBACKGROUND :

		       /* Have OS/2 Presentation Manager perform the	*/
		       /* background erase on behalf of	the button	*/

       return(MRFROMLONG(TRUE));

   /*********************************************************************/
   /*  Paint the button							*/
   /*********************************************************************/

   case	WM_PAINT :
		       /* Get the address of the private data from the	*/
		       /* control's reserved memory                     */

       ptf = (PTEXTFIELD)WinQueryWindowPtr(hWnd, QUCWP_WNDP);

		       /* Get the presentation space for the control	*/
		       /* and set the colour table to RGB mode		*/

       GpiCreateLogColorTable(hPS = WinBeginPaint(hWnd,	(HPS)NULL, (PRECTL)NULL),
			      0L, LCOLF_RGB, 0L, 0L, (PLONG)NULL);

       GpiSetColor(hPS,	ptf->lClrLeftTop);

       GpiMove(hPS, ptf->aptl);
       GpiPolyLine(hPS,	2L, &ptf->aptl[1]);

       GpiSetColor(hPS,	ptf->lClrBottomRight);

       GpiMove(hPS, &ptf->aptl[2]);
       GpiPolyLine(hPS,	2L, &ptf->aptl[3]);

       WinEndPaint(hPS);
       break;

/************************************************************************/
/************************************************************************/
/*									*/
/* Part	6: Control destruction coding					*/
/*									*/
/************************************************************************/
/************************************************************************/

   /*********************************************************************/
   /*  Window being destroyed, perform clean-up				*/
   /*********************************************************************/

   case	WM_DESTROY :
		       /* Get the address of the private data from the	*/
		       /* control's reserved memory                     */

       ptf = (PTEXTFIELD)WinQueryWindowPtr(hWnd, QUCWP_WNDP);
       WinDestroyWindow(ptf->hwndText);

		       /* Release the heap allocated for use by	the	*/
		       /* control					*/

       DosFreeMem((PVOID)ptf);
       break;
		       /* Default message processing			*/
   default :
       return(WinDefWindowProc(hWnd, msg, mp1, mp2));
   }

return(0L);
}
