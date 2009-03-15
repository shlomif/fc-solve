/******************************************************************************\
* wilson callan, copyright 1997
\******************************************************************************/

#if 0
#include <windows.h>
#endif
#include <stdio.h>

#include "resource.h"
#include "mainfns.h"

#define MAXSTRING                   MAX_PATH

#define PUBLIC
#if !DBG
#define PRIVATE static
#else
#define PRIVATE
#endif

#define MAINAPPNAME      "Main"
#define MAINCLASSNAME	 "Main Class"
#define STATUSCLASSNAME	 "Status Class"

#define FILTERNAME1 "Text Files (*.txt)"
#define FILTER1		"*.TXT"
#define FILTERNAME2 "All Files (*.*)"
#define FILTER2		"*.*"

typedef struct 
{
	CHAR szFilterName1[sizeof (FILTERNAME1)];	// has to be exact size
	CHAR szFilter1[sizeof (FILTER1)];
	CHAR szFilterName2[sizeof (FILTERNAME2)];
	CHAR szFilter2[sizeof (FILTER2)];
	CHAR end;

} FILTER;

#define NUMSTATUSERS 10
#define MAX_STATUSERNAME 16
struct
{
	struct
	{
		UINT CurrW ;
		UINT CurrL ;
		UINT TotalW ;
		UINT TotalL ;
		UINT StreaksW ;
		UINT StreaksL ;
		INT  StreaksC ;
		CHAR UserName[MAX_STATUSERNAME] ;
	}	Stats[NUMSTATUSERS] ;
	INT CurStatUser ;
	BOOL StatEnbl ;
	BOOL StatAlert ;
	BOOL StatQuitConfirm ;
} StatsMU ;
/* the height of the toolbar in pels. */

#define TOOLBARHEIGHT 30

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, INT nCmdShow);
VOID MainStartGame (BOOL bDeal);
VOID InvalidateMain (LPRECT rc);
VOID MainOut (INT x, INT y, CHAR *s, INT d);
VOID MainMessage (CHAR *s);
VOID UpdateMain (VOID);
VOID StatusOut (CHAR *str);

// main.c

extern HANDLE ghInst;
extern int FAR gnWidth, gnHeight, gnDefWidth, gnDefHeight;
extern int FAR gnNorWidth, gnNorHeight, gnRedWidth, gnRedHeight;
extern int gnToolbar;
extern UCHAR gszWindowName[];
extern WINDOWPLACEMENT gwndpl;
extern DWORDLONG gnGameNumber, gnOGameNumber ;
extern BOOL gbGameOn, gbHumanPlaying, gbSpider, gbFastSolve ;
extern INT SCUser, gnDblclkMode ;
extern INT gnLoadMoves;
extern INT gnFCRed, gnFCGreen, gnFCBlue;
extern INT gnBGRed, gnBGGreen, gnBGBlue;
extern HWND ghwndMainApp;
extern INT gnSolveS, gnMaxTimeS, gnMaxHandsS ;
extern char szPresortCS[3] ;

extern HBITMAP hBitmapC ;

// file.c

extern CHAR gszLoadFile[MAXSTRING];
extern FILTER gFilters;
extern BOOL gbLoadingSolution, gbSolutionLoaded;

// free1.c

extern INT gnSolve;
extern DWORDLONG gnFirstGame, gnLastGame ;
extern INT gnMaxHands, gnMaxTime ;
extern int gnExtendTry ;

