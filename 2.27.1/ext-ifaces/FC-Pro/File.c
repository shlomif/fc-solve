// wilson callan 1998

#include "main.h"
#include "custom.h"
#include "freecell.h"
#include "log.h"
#include "settings.h"
#include "fcplay.h"

CHAR gszLoadFile[MAXSTRING];
BOOL  gbLoadingSolution, gbSolutionLoaded;
PRIVATE CHAR  gszSaveFile[MAXSTRING];
PRIVATE BOOL LoadChar (CHAR cMove);
extern INT gnUserMoves;
extern BOOL gbSolver ;
BOOL gbSearchF, gbSearchF2, gbSearchF3, gbSearchFGend, gbSearchFC, gbSearchFR ;
HFILE fhFileSF ;
BOOL SearchFNxtGame(HWND) ;
BOOL SearchFNxtMove(HWND) ;
INT nGameNoValid ; 
BOOL PlayMode2 ;
//INT MoveCount, GameCount ;
extern int cdMoveCnt, FcMode ;
extern BOOL cdMoveCnt2 ;
extern BOOL gbStarted ;

FILTER gFilters = {FILTERNAME1, FILTER1, FILTERNAME2, FILTER2, (CHAR) NULL};

VOID FileInit ()
{
	if (gbCustomGame)
	{
		sprintf (gszSaveFile, "%s.txt", gszCustomGameName);
	}
	else
	{
		sprintf (gszSaveFile, "%s.txt", FmtGameNo(gnGameNumber));
	// cant init this here since its called by LoadSolution()
	//	sprintf (gszLoadFile, "%d.txt", gnGameNumber);
	// could init it in main.c before dialog.c is called
	}

	gbLoadingSolution = FALSE;
	gbSolutionLoaded  = FALSE;
}

BOOL SaveSolution (HWND hwnd)
{
	HFILE fhFile;
	OPENFILENAME ofn;
	CHAR  szTemp[MAXSTRING+17];
	DWORD rtn;

    ofn.lStructSize			= sizeof(OPENFILENAME); 
    ofn.hwndOwner			= hwnd; 
    ofn.hInstance			= ghInst; 
    ofn.lpstrFilter			= (LPSTR) &gFilters;  
    ofn.lpstrCustomFilter	= NULL; 
    ofn.nFilterIndex		= 0; 
    ofn.lpstrFile			= gszSaveFile; 
	ofn.nMaxFile			= MAXSTRING;
    ofn.lpstrFileTitle		= NULL; 
    ofn.lpstrInitialDir		= NULL; 
    ofn.lpstrTitle			= NULL; 
    ofn.Flags				= OFN_HIDEREADONLY | OFN_EXPLORER | 
							  OFN_OVERWRITEPROMPT; 
    ofn.lpstrDefExt			= NULL; 

	if (!GetSaveFileName (&ofn))
	{
		// examine results

		if (rtn = CommDlgExtendedError())
		{
			// error wasnt just because user hit cancel!

			// 9 == CDERR_MEMALLOCFAILURE

			sprintf (szTemp, "ERROR_GETSAVEFILENAME (0x%04x).", rtn);
			Message (hwnd, 0, szTemp);
		}

		return FALSE;
	}

    if ((fhFile = _lcreat(gszSaveFile, 0)) == HFILE_ERROR)
	{
		//put up a message here.
		sprintf (szTemp, "ERROR_LCREAT: '%s'", gszSaveFile);
		ShowLastError (hwnd, szTemp);
		return FALSE;
	}

	ShowSolution (NULL, fhFile, NULL, FALSE);

	_lclose(fhFile);

	sprintf (szTemp, "Moves Saved in %s.", gszSaveFile); 
	StatusOut (szTemp); 

	return TRUE;
}

BOOL SaveSolutionFast (HWND hwnd)
{
	HFILE fhFile;
	CHAR  szTemp[MAXSTRING+17];

	// never overwrite without asking

	if ((fhFile = _lopen(gszSaveFile, OF_WRITE)) != HFILE_ERROR)
	{
		// File exists - confirm overwrite

		sprintf (szTemp, "Overwrite '");
		strncat (szTemp, gszSaveFile, MAXSTRING);
		strcat (szTemp, "'?");

		if (Message (hwnd, MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2, 
					szTemp) != IDYES)
		{
			return FALSE;
		}
	}

    if ((fhFile = _lcreat(gszSaveFile, 0)) == HFILE_ERROR)
	{
		//put up a message here.
		sprintf (szTemp, "ERROR_LCREAT: '%s'", gszSaveFile);
		ShowLastError (hwnd, szTemp);
		return FALSE;
	}

	ShowSolution (NULL, fhFile, NULL, FALSE);

	_lclose(fhFile);

	sprintf (szTemp, "Moves Saved in %s.", gszSaveFile); 
	StatusOut (szTemp); 

	return TRUE;
}

BOOL SaveAuto (HWND hwnd)
{
	HFILE fhFile;
	OPENFILENAME ofn;
	CHAR  szFile[MAXSTRING];
	CHAR  szTemp[MAXSTRING+17];
	DWORD rtn;

	sprintf (szFile, "%saut.txt", FmtGameNo(gnGameNumber));

    ofn.lStructSize			= sizeof(OPENFILENAME); 
    ofn.hwndOwner			= hwnd; 
    ofn.hInstance			= ghInst; 
    ofn.lpstrFilter			= (LPSTR) &gFilters;  
    ofn.lpstrCustomFilter	= NULL; 
    ofn.nFilterIndex		= 0; 
    ofn.lpstrFile			= szFile; 
	ofn.nMaxFile			= MAXSTRING;
    ofn.lpstrFileTitle		= NULL; 
    ofn.lpstrInitialDir		= NULL; 
    ofn.lpstrTitle			= "Save Moves with Auto Moves - Debug only!"; 
    ofn.Flags				= OFN_HIDEREADONLY | OFN_EXPLORER | 
							  OFN_OVERWRITEPROMPT; 
    ofn.lpstrDefExt			= NULL; 

	if (!GetSaveFileName (&ofn))
	{
		// examine results

		if (rtn = CommDlgExtendedError())
		{
			// error wasnt just because user hit cancel!

			// 9 == CDERR_MEMALLOCFAILURE

			sprintf (szTemp, "ERROR_GETSAVEFILENAME (0x%04x).", rtn);
			Message (hwnd, 0, szTemp);
		}

		return FALSE;
	}

    if ((fhFile = _lcreat(szFile, 0)) == HFILE_ERROR)
	{
		//put up a message here.
		sprintf (szTemp, "ERROR_LCREAT: '%s'", szFile);
		ShowLastError (hwnd, szTemp);
		return FALSE;
	}

	ShowSolution (NULL, fhFile, NULL, TRUE);

	_lclose(fhFile);

	sprintf (szTemp, "Moves with Auto Moves Saved in %s.", szFile); 
	StatusOut (szTemp); 

	return TRUE;
}

VOID OpenSolution (HWND hwnd)
{
	OPENFILENAME ofn;
	CHAR  szFile[MAXSTRING];
	CHAR  szTemp[MAXSTRING+8];
	DWORD rtn;

	sprintf (szFile, "%s.txt", FmtGameNo(gnGameNumber));

    ofn.lStructSize			= sizeof(OPENFILENAME); 
    ofn.hwndOwner			= hwnd; 
    ofn.hInstance			= ghInst; 
    ofn.lpstrFilter			= (LPSTR) &gFilters;  
    ofn.lpstrCustomFilter	= NULL; 
    ofn.nFilterIndex		= 0; 
    ofn.lpstrFile			= szFile; 
	ofn.nMaxFile			= MAXSTRING;
    ofn.lpstrFileTitle		= NULL; 
    ofn.lpstrInitialDir		= NULL; 
    ofn.lpstrTitle			= NULL; 
    ofn.Flags				= OFN_HIDEREADONLY | OFN_EXPLORER;
    ofn.lpstrDefExt			= NULL; 

	if (!GetOpenFileName (&ofn))
	{
		// examine results

		if (rtn = CommDlgExtendedError())
		{
			// error wasnt just because user hit cancel!

			// 9 == CDERR_MEMALLOCFAILURE

			sprintf (szTemp, "ERROR_GETOPENFILENAME (0x%04x).", rtn);
			Message (hwnd, 0, szTemp);
		}

		return;
	}

	// open notepad with this file for edit

	sprintf (szTemp, "NotePad ");
	strncat (szTemp, ofn.lpstrFile, MAXSTRING);
	WinExec (szTemp, SW_SHOWNORMAL);
}

//
// returns new game number
//
VOID LoadSolution (HWND hwnd, BOOL fLogging)
{
	HFILE fhFile;
	CHAR  szTemp[46];
//	CHAR  szMove[3];
	CHAR  szMove[4];
	INT   nAttemptNumber,nLoadMoves, nONumFcs, nGameNoValid, i ;
	DWORDLONG nGameNumber ;	
	BOOL  bContinuous;
	PAINTSTRUCT pnt ;
	nGameNoValid = 1 ;
	if ((fhFile = _lopen(gszLoadFile, OF_READ)) == HFILE_ERROR)
	{
		//put up a message here.
		sprintf (szTemp, "ERROR_LOPEN: '%s'", gszLoadFile);
		ShowLastError (hwnd, szTemp);
		return;
	}

	// File exists
	_llseek(fhFile, 0, 0) ;
	_lread(fhFile, szTemp, 5) ;
	gbSearchFR = FALSE ;
	if (szTemp[3] == 'R')
		gbSearchFR = TRUE ;
	szTemp[3] = 0 ;
	if (szTemp[0] == 'F')
		PlayMode2 = TRUE ;
	gbSearchF = gbSearchF2 = gbSearchF3 = FALSE ;
	if (!lstrcmp(szTemp, "&&&"))
	{
		fhFileSF = fhFile ;
		gbSearchF = TRUE ;
					//A multi-solution file to be search for False Impossibles
		PostMessage(hwnd, WM_USER+104, 0, 0L) ;  // Seems to need to be timer-driven 
		return ;
	}
	if (!lstrcmp(szTemp, "~~~"))
	{
		fhFileSF = fhFile ;
		gbSearchF2 = TRUE ;  //for Solver2  
					//A multi-solution file to be search for False Impossibles
		PostMessage(hwnd, WM_USER+106, 0, 0L) ;  // Seems to need to be timer-driven 
		return ;
	}
	if (!lstrcmp(szTemp, "^^^"))
	{
		fhFileSF = fhFile ;
		gbSearchF3 = TRUE ;  //for Solver3  
					//A multi-solution file to be search for False Impossibles
//		GameCount = 0 ;
		PostMessage(hwnd, WM_USER+107, 0, 0L) ;  // Seems to need to be timer-driven 
		return ;
	}
	// scan for the last game number in file
	_llseek(fhFile, 0, FILE_END);

	if (!FileScan (fhFile, '#', -1))
	{
		Message (hwnd, 0, "No Game Number in solution file. (1)");
		_lclose(fhFile);
		return;
	}

	// found '#', scan in game number
	_lread(fhFile, szTemp, 10);

//	if (sscanf (szTemp, "%ld", &nGameNumber) != 1)
	for (i = 0 ; i < 10 ; i++)
		if (!isdigit(szTemp[i]))
		{
			szTemp[i] = 0 ;
			break ;
		}
	if (!i)
	{
		Message (hwnd, 0, "No Game Number in solution file.\nWill apply to current position.");
		nGameNoValid = 0 ;
//		_lclose(fhFile);
//		return;
	}
	nGameNumber = CvGameNo(szTemp) ;

	// backup to # again

	if (!FileScan (fhFile, '#', -1))
	{
		Message (hwnd, 0, "Lost Game Number in solution file!");
		_lclose(fhFile);
		return;
	}

	// scan to ':'
	if (!FileScan (fhFile, ':', 1))
	{
		Message (hwnd, 0, "No Attempt Number in load file (1).  Continuing...");
		nAttemptNumber = 1;

		// return to last #
		FileScan (fhFile, '#', -1);
	}
	else
	{
		// found ':', scan in attempt number
		_lread(fhFile, szTemp, 10);	  // assume 9999999999 max
		if (sscanf (szTemp, "%ld", &nAttemptNumber) != 1)
		{
			Message (hwnd, 0, "No Attempt Number in load file (2).  Continuing...");
			nAttemptNumber = 1;
		}
	_llseek(fhFile, -10, 1) ;  // back up to attempt no.
	}

// scan to '='
	if (!FileScan (fhFile, '=', 1))
	{
		// return to last #
		FileScan (fhFile, '#', -1);
	}
	else
	{
		// found '=', scan in no. of freecells
		nONumFcs = NumFcs ;
		_lread(fhFile, szTemp, 3);	  
		NumFcs = szTemp[0] - '0' ;
		if ((isdigit(szTemp[0])) && (szTemp[0] >= '0') && (szTemp[0] <= '6'))
		NumFcs = szTemp[0] - '0' ;
		if ((szTemp[1] == '/') && (szTemp[2] == 'S'))
			gbSpider = TRUE ;
		else
			gbSpider = FALSE ;
		if (NumFcs != nONumFcs)
			FCPlayReInit(hwnd) ;
	}

	// deal hand
	if (nGameNoValid) 
	{
		gnGameNumber = nGameNumber;
		FreeCellInit (TRUE);
		sprintf (szTemp, "Game #%s", FmtGameNo(gnGameNumber));
		SetMainCaption (ghwndMainApp, szTemp);
	}

	// scan to first move

	if (!FileScan (fhFile, LF, 1))
	{
		Message (hwnd, 0, "No Moves in solution file.");
		_lclose(fhFile);
		return;
	}

	// open log file

	if (fLogging)
	{
		StopLogging ();
		StartLogging (hwnd, gszLoadFile, nAttemptNumber);
	}

	// let freecell logic know what is going on

	gbLoadingSolution = TRUE;

	if (!nGameNoValid)
	{
		ShowHand(&pnt) ;	
		InvalidateMain(NULL) ;
		gbCustomGame = TRUE ;
	}

	// send moves thru ringer!

	bContinuous = FALSE;
	nLoadMoves = 0;

	while (TRUE)
	{
		// keep reading until we get a 1st move card
		// this gets us over spaces and crlf
		while (TRUE)
		{
			if (_lread(fhFile, szMove, 1) == 0)
			{
				// end of file
				gbLoadingSolution = FALSE;
				break;
			}

			if (LoadChar (szMove[0]))
			{
				// got our 1st move
				break;
			}
			else if (szMove[0] == 'L')
			{
				// we are loading log file, we are done
				gbLoadingSolution = FALSE;
				break;
			}
		}

		if (!gbLoadingSolution)
		{
			// got eof or loading log file

			break;
		}

		// load 2nd move

		if (_lread(fhFile, &szMove[1], 1) == 0)
		{
			// end of file
			break;
		}

		if (szMove[1] == 'L')
		{
			// we are loading log file, we are done
			break;
		}
		
		if ((gnLoadMoves >= 0) &&   // user didnt leave -1 in dialog
			(++nLoadMoves > gnLoadMoves) && 
			!bContinuous) // user hasnt already selected continuous
		{
			sprintf (szTemp, "Next Move is %c->%c%sLoad solution continuously?",
						szMove[0], szMove[1], CRLF);

			switch (Message (hwnd, 
						MB_ICONQUESTION | MB_YESNOCANCEL | MB_DEFBUTTON2, 
						szTemp))
			{
				case IDYES:
					bContinuous = TRUE;
					break;

				case IDCANCEL:
					gbLoadingSolution = FALSE;
					break;
			}
		}

		if (!gbLoadingSolution)
		{
			// got cancel
			break;
		}
		gbGameOn = TRUE ;
		SendMessage (hwnd, WM_CHAR, (WORD) szMove[0], 0);

		if (!gbLoadingSolution)
		{
			// got illegal move (previous move left card
			// selected)
			break;
		}
if (cdMoveCnt != 0)
	{
	PlayMode2 = TRUE ;
	FcMode = 1 ;
}
if (PlayMode2)
{		
		if (_lread(fhFile, &szMove[2], 1) == 0)
			{
				// end of file
				gbLoadingSolution = FALSE;
				break;
			}
		 cdMoveCnt = 0 ;
		 cdMoveCnt2 = FALSE ;
		 if (isalnum((int)szMove[2]))
		 {
			if (isdigit((int)szMove[2]))
			 cdMoveCnt = szMove[2] - '0' ;
			else
			 cdMoveCnt = szMove[2] - 'a' + 10 ;
//			 MainMessage("Found Digit") ;
		 }
 }

		SendMessage (hwnd, WM_CHAR, (WORD) szMove[1], 0);
		if (!gbLoadingSolution)
		{
			// got illegal move 
			break;
		}
	}

	gbLoadingSolution = FALSE;

	// reached end of file! (or got an illegal move)

	_lclose(fhFile);
    PlayMode2 = FALSE ;
	gbSolutionLoaded = TRUE;
	gbStarted = TRUE ;
	return;
}

PRIVATE BOOL LoadChar (CHAR cMove)
{
	if ((cMove >= '1' && cMove <= '8') ||
//		(cMove >= 'a' && cMove <= 'd') ||
		(cMove >= 'a' && cMove <= 'g') ||
#ifndef FC89
		cMove == 'h')
#else
		(cMove == 'h') || ((cMove >= 'k') && (cMove <= 'l'))) 
#endif
	{
		return TRUE;
	}

	return FALSE;
}

/*****************************************************************************\
*
* Copies all lines to the clipboard in text format.
*
* Arguments:
*   none
*
* Returns:
*   TRUE if successful, FALSE if not.
*
\*****************************************************************************/
BOOL CopySolution (HWND hwnd)
{
    INT cch;
    LPSTR pBuf = NULL;
    BOOL fSuccess = FALSE;

	// figure how much memory we need

	cch = STD_MAX_CHARS_TOP_LINE + 
		((gnUserMoves)+1)*3 + // moves * (card,card,space)
		(gnUserMoves/STD_MOVES_PER_LINE)*3; // lines * (cr,lf,NULL)

	// get memory

    if (!(pBuf = (LPSTR) GlobalAlloc(GMEM_DDESHARE, cch * sizeof(TCHAR))))
    {
		ShowLastError (hwnd, "ERROR_GLOBALALLOC");
        return FALSE;
    }

	// load memory

	ShowSolution (NULL, (HFILE) NULL, pBuf, FALSE);

	// fill up clipboard

    if (OpenClipboard (hwnd))
    {
        EmptyClipboard ();
        if (!(fSuccess = 
				SetClipboardData(CF_TEXT, pBuf) ? TRUE : FALSE))
		{
			// putup message
			ShowLastError (hwnd, "ERROR_SETCLIPBOARDDATA");
		}

        CloseClipboard();
    }

    return fSuccess;
}

HFILE fhFile ;

LONGLONG GetSolveGame(int mode)
{
//HFILE (fhFile) ;
int i, cc ;
char szSolvBuf[20] ;
//char szTemp[25] ;
if (!mode)
{
//MainMessage("GSG Start") ;
	lstrcpy(gszLoadFile, "solvlist.txt") ;
	if ((fhFile = _lopen(gszLoadFile, OF_READ)) == HFILE_ERROR)
	{
		MainMessage("Solvlist.txt file not present") ;
		return -1 ;
	}
	_lread(fhFile, szSolvBuf, 12) ;
	if (!isdigit(szSolvBuf[0]))
	{
		if (!FileScan(fhFile, LF, 1))
		{
			MainMessage("No data in file.") ;
			_lclose(fhFile) ;
			return -1 ;  // End of file
		}
		cc = _lread(fhFile, szSolvBuf, 15) ;
		for (i = 0 ; i < 5 ; i++)
		{
			if (isdigit(szSolvBuf[i]))
			break ;
		}
		if (i == 5)
		{
			MainMessage("No data in file.") ;
			_lclose(fhFile) ;
			return -1 ;  // No more numbers
		}
		szSolvBuf[10 + i] = 0 ;  //Safety stop
		if (cc < 2)
			MainMessage("Solvlist at end") ;
		_llseek(fhFile, -cc + 1, 1) ; // back up for next scan
//		return(atol(szSolvBuf + i)) ;
		return(_atoi64(szSolvBuf + i)) ;
	}
	szSolvBuf[10] = 0 ;  //Safety stop
	_llseek(fhFile, 0, 0) ;  // return file pointer to start
//	return(atol(szSolvBuf)) ;
	return(_atoi64(szSolvBuf)) ;
}
else
{
	if (!FileScan(fhFile, LF, 1))
	{
		_lclose(fhFile) ;
		return -1 ;  // End of file
	}
	cc = _lread(fhFile, szSolvBuf, 15) ;
	for (i = 0 ; i < 5 ; i++)
	{
		if (isdigit(szSolvBuf[i]))
			break ;
	}
	if (i == 5)
	{
		_lclose(fhFile) ;
		return -1 ;  // No more numbers
	}
	szSolvBuf[10 + i] = 0 ;  //Safety stop
	_llseek(fhFile, -cc + 1, 1) ; // back up for next scan
//	if (!atol(szSolvBuf + i))
	if (!_atoi64(szSolvBuf + i))
	{
		_lclose(fhFile) ;
		return -1 ;	  //Don't return 0 from empty line
	}
//	return(atol(szSolvBuf + i)) ;
	return(_atoi64(szSolvBuf + i)) ;
 }
return -1 ;
}

BOOL SearchFNxtGame(HWND hwnd)
{
	CHAR  szTemp[46];
	INT   nONumFcs, i ;
	DWORDLONG nGameNumber ;	
	PAINTSTRUCT pnt ;
//	LONG fPointer ;
//	CHAR szMove[2] ;
	nGameNoValid = 1 ;
	
//	MoveCount = 0 ;
//	GameCount++ ;
	if (!FileScan (fhFileSF, '#',1))
	{
		if (!gbSearchFR)
		{
			gbLoadingSolution = FALSE;
			_lclose(fhFile);
			return 0;
		}
		else
//MessageBox(GetFocus(), "Repeat", "GN", MB_OK) ;
			_llseek(fhFileSF, 0, 0) ;
			if (!FileScan (fhFileSF, '#',1))
			{
				gbLoadingSolution = FALSE;
				_lclose(fhFile);
				return 0;
			}	
		}
	// found '#', scan in game number
	_lread(fhFileSF, szTemp, 10);
//MessageBox(GetFocus(), szTemp, "GN", MB_OK) ;

	for (i = 0 ; i < 10 ; i++)
		if (!isdigit(szTemp[i]))
		{
			szTemp[i] = 0 ;
			break ;
		}
	if (!i)
		return 0 ;
	nGameNumber = CvGameNo(szTemp) ;
// scan to '='
	
	if (!FileScan (fhFileSF, '=', 1))
	{
		// return to last #
		FileScan (fhFileSF, '#', -1);
	}
	else
	{
		// found '=', scan in no. of freecells
		nONumFcs = NumFcs ;
		_lread(fhFileSF, szTemp, 3);	  
		NumFcs = szTemp[0] - '0' ;
		if ((isdigit(szTemp[0])) && (szTemp[0] >= '0') && (szTemp[0] <= '6'))
		NumFcs = szTemp[0] - '0' ;
		if (NumFcs != nONumFcs)
			FCPlayReInit(hwnd) ;
	}
	_llseek(fhFileSF, -3, 1) ;
	// deal hand
	if (nGameNoValid) 
	{
		gnGameNumber = nGameNumber;
		FreeCellInit (TRUE);
		sprintf (szTemp, "Game #%s", FmtGameNo(gnGameNumber));
		SetMainCaption (ghwndMainApp, szTemp);
	}
	if (!FileScan (fhFileSF, LF, 1))
	{
		Message (hwnd, 0, "No Moves in solution file.");
		_lclose(fhFile);
		return 0;
	}

	// let freecell logic know what is going on

	gbLoadingSolution = TRUE;

	if (!nGameNoValid)
	{
		ShowHand(&pnt) ;	
		InvalidateMain(NULL) ;
		gbCustomGame = TRUE ;
	}
    SearchFNxtMove(hwnd) ;
	return TRUE ;
}

BOOL SearchFNxtMove(HWND hwnd)
	// scan to first move
{
	CHAR  szMove[3];
	
	// send moves thru ringer!
	//	while (TRUE)
//	{
		// keep reading until we get a 1st move card
		// this gets us over spaces and crlf
//MoveCount++ ;
	while (TRUE)
		{
			if (_lread(fhFileSF, szMove, 1) == 0)
			{
				// end of file
				gbLoadingSolution = FALSE;
				return 0 ;
			}

			if (LoadChar (szMove[0]))
			{
				// got our 1st move
				break;
			}
			else if (szMove[0] == 'L')
			{
				// we are loading log file, we are done
				gbLoadingSolution = FALSE;
				return 0 ;
			}
		}

		if (!gbLoadingSolution)
		{
			// got eof or loading log file
			return 0 ;
		}

		// load 2nd move

		if (_lread(fhFileSF, &szMove[1], 1) == 0)
		{
			// end of file
			return 0;
		}
		if (szMove[1] == 'L')
		{
			// we are loading log file, we are done
			return 0;
		}

		if (!gbLoadingSolution)
		{
			// got cancel
			return 0;
		}
		gbGameOn = TRUE ;
		SendMessage (hwnd, WM_CHAR, (WORD) szMove[0], 0);

		if (!gbLoadingSolution)
		{
			// got illegal move (previous move left card
			// selected)
			return 0;
		}
//MessageBox(GetFocus(), "6", "SFNA", MB_OK) ;

		SendMessage (hwnd, WM_CHAR, (WORD) szMove[1], 0);
		if (!gbLoadingSolution)
		{
			// got illegal move 
			return 0;
		}
//		SendMessage (hwnd, WM_COMMAND, (WORD) ID_SOLVEFAST, 0);
//		PostMessage (hwnd, WM_COMMAND, (WORD)WM_USER+105, 0);
//	}
	return TRUE ;
}

VOID SearchFChk1Start(HWND hwnd)
{
		gbSearchFC = gbSearchF = TRUE ;  //for Solver1  
		PostMessage(hwnd, WM_USER+108, 0, 0L) ;  // Seems to need to be timer-driven 
		return ;
}


VOID SearchFChk2Start(HWND hwnd)
{
		gbSearchFC = gbSearchF2 = TRUE ;  //for Solver2  
		PostMessage(hwnd, WM_USER+109, 0, 0L) ;  // Seems to need to be timer-driven 
		return ;
}

VOID SearchFChk3Start(HWND hwnd)
{
		gbSearchFC = gbSearchF3 = TRUE ;  //for Solver3  
		PostMessage(hwnd, WM_USER+110, 0, 0L) ;  // Seems to need to be timer-driven 
		return ;
}

BOOL SearchFCNxtMove(HWND hwnd)
{
	if (gnSels != 0)
		SendMessage(hwnd, WM_CHAR, VK_BACK, 0L) ;
	else
		return FALSE ;
	return TRUE ;
}
