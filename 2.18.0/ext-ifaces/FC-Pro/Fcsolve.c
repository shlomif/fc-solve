// copyright wilson callan 1997

#include "main.h"
#include "FCSolve.h"
#include "settings.h"
#include "freecell.h"
#include "log.h"
#include "custom.h"

#define DEBUG	(0)

#define PRESORT (1)

//PRIVATE INT  gnFCSolveIndex; 
INT  gnFCSolveIndex; 
int gnFCSolveIndexC ;
//PRIVATE char FCSolveMoves[STD_MOVES_PER_LINE * STD_MAX_LINES * AUTO_PER_MOVE];
char FCSolveMoves[STD_MOVES_PER_LINE * STD_MAX_LINES * AUTO_PER_MOVE];
BOOL gbLoadingSolver, gbSolverLoaded, gbSolver2 ;
extern BOOL gbSolver ;
extern BOOL gbCustomGame ;
extern char gszCustomGameName[CUSTOM_MAXSTRING_NAME] ;
extern BOOL PlayMode2 ;

PRIVATE INT ColSeq[MAXCOL - 1] ;
char ColSeqC[MAXCOL - 1] ;
char PresortC[5] ;
int RotCnt ;

VOID FCSolveInit (VOID)
{
	gnFCSolveIndex = 0;
}

BOOL FCSolveMove (char ch)
{
	if (gnFCSolveIndex > (STD_MOVES_PER_LINE * STD_MAX_LINES * AUTO_PER_MOVE - 2))
	{
		MainMessage("Winnable, but....\nSolution is too long\nto be replayed.") ;
		return(FALSE) ;
	}
	FCSolveMoves[gnFCSolveIndex++] = ch;
	return(TRUE) ;
}

#define DEBUG2 1

VOID FCSolvePlay (HWND hwnd)
{
	INT  i;
	// read moves out backwards starting with last which
	// is one before current index

extern int SupMove, FcMode ;
char szTemp[200] ;
BOOL SaveFlag1, SaveFlag2, SaveFlag3 ;
	gnFCSolveIndex--;
	SaveFlag1 = gbSolver ;
	SaveFlag2 = gbSolver2 ;
	SaveFlag3 = gbSolver3 ;
#if DEBUG2
#ifndef FC89
	if (gbSolver2)
#else
	if (gbSolver || gbSolver2 || gbSolver3)
#endif
	{
		FILE *fpFile;
		BYTE j ;

//		fpFile = fopen ("FCSolve.txt", "w");
if (gbSolver2)
{
		fpFile = fopen ("FCSSolutionD.txt", "w");
	if (!gbCustomGame)
		sprintf (szTemp, "#%s Att.:1 NumFcs=%d (FCS Solution)%s", 
				FmtGameNo(gnGameNumber), NumFcs, CRLF);
	else
		sprintf (szTemp, "#%s Att.:1 NumFcs=%d (FCS Solution)%s", 
				gszCustomGameName, NumFcs, CRLF);
}
if (gbSolver)
{
		fpFile = fopen ("FCSolveD.txt", "w");
	if (!gbCustomGame)
		sprintf (szTemp, "#%s Att.:1 NumFcs=%d (FCPro - FCSolve debug)%s", 
				FmtGameNo(gnGameNumber), NumFcs, CRLF);
	else
		sprintf (szTemp, "#%s Att.:1 NumFcs=%d (FCPro - FCSolve debug)%s", 
				gszCustomGameName, NumFcs, CRLF);
}
if (gbSolver3)
{
		fpFile = fopen ("FCPatSolveD.txt", "w");
	if (!gbCustomGame)
		sprintf (szTemp, "#%s Att.:1 NumFcs=%d (Patsolve Solution)%s", 
				FmtGameNo(gnGameNumber), NumFcs, CRLF);
	else
		sprintf (szTemp, "#%s Att.:1 NumFcs=%d (Patsolve Solution)%s", 
				gszCustomGameName, NumFcs, CRLF);
}
		fprintf (fpFile, szTemp);

		for (i = gnFCSolveIndex, j = 0; i >= 0; i -= 2)
		{
			fprintf (fpFile, "%c%C ", 
						FCSolveMoves[i], FCSolveMoves[i - 1]);

			if ((++j % STD_MOVES_PER_LINE) == 0)
			{
				fprintf (fpFile, "\n");
			}
		}
		fclose (fpFile);
	}
#endif
	// let freecell logic know what is going on

	gbLoadingSolver = TRUE;

	// send moves to fcpro
//MainMessage(FCSolveMoves) ;

	for (i = gnFCSolveIndex; i >= 0; i--)
	{
#ifdef PRESORT
		if (isdigit((int)FCSolveMoves[i]) && isalpha(PresortC[0]))
			FCSolveMoves[i] = strchr(ColSeqC, (int)FCSolveMoves[i]) - ColSeqC + '1' ;
#endif
//sprintf(szTemp, "ch- %c %c %d %s", FCSolveMoves[i+1], FCSolveMoves[i], gbSolver3, FCSolveMoves) ;
//MessageBox(GetFocus(), szTemp, "SvPl", MB_OK) ;
		if (FCSolveMoves[i]	!= 'x')
			SendMessage (hwnd, WM_CHAR, (WORD) FCSolveMoves[i], 0);

		if (!gbLoadingSolver)
		{
			if (FCSolveMoves[i] == 'h')
			{
				// its ok, probably an autopost.
				// note that an autopost dont result in 
				// an error if the autopost emptied the column.
				// deselect the selected card.

				SendMessage (hwnd, WM_CHAR, (WORD) 
								FCSolveMoves[i + 1], 0);

				gbLoadingSolver = TRUE;
			}
			else
			{
//sprintf(szTemp, "ch- %c %c %s", FCSolveMoves[i+1], FCSolveMoves[i], FCSolveMoves) ;
//MessageBox(GetFocus(), szTemp, "SvPl", MB_OK) ;
				// got illegal move 
				if (!gbSolver2)
					Message (hwnd, MB_ICONEXCLAMATION, "Illegal Move by Solver."); 
				else
Message (hwnd, MB_ICONEXCLAMATION, "This solution cannot be replayed\nby FcPro because of an incompatibility\nbetween Freecell Solver and FcPro.\nThe full solution may be seen\nin file FCSSolution.txt") ;
				break;
			}
		}
		gnFCSolveIndexC = i - 2 ;
	}

	gbLoadingSolver = FALSE; 

//MainMessage("Play fini") ;
	if (SaveFlag1 || SaveFlag2 || SaveFlag3)
	{
		FILE *fpFile;
		BYTE j ;

//		fpFile = fopen ("FCSolve.txt", "w");
if (SaveFlag2)
{
		fpFile = fopen ("FCSSolution.txt", "w");
	if (!gbCustomGame)
		sprintf (szTemp, "F#%s Att.:1 NumFcs=%d (FCS Solution)%s", 
				FmtGameNo(gnGameNumber), NumFcs, CRLF);
	else
		sprintf (szTemp, "F#%s Att.:1 NumFcs=%d (FCS Solution)%s", 
				gszCustomGameName, NumFcs, CRLF);
}
if (SaveFlag1)
{
		fpFile = fopen ("FCSolve.txt", "w");
	if (!gbCustomGame)
		sprintf (szTemp, "#%s Att.:1 NumFcs=%d (FCPro - FCSolve debug)%s", 
				FmtGameNo(gnGameNumber), NumFcs, CRLF);
	else
		sprintf (szTemp, "#%s Att.:1 NumFcs=%d (FCPro - FCSolve debug)%s", 
				gszCustomGameName, NumFcs, CRLF);
}
if (SaveFlag3)
{
		fpFile = fopen ("FCPatSolve.txt", "w");
	if (!gbCustomGame)
		sprintf (szTemp, "#%s Att.:1 NumFcs=%d (Patsolve Solution)%s", 
				FmtGameNo(gnGameNumber), NumFcs, CRLF);
	else
		sprintf (szTemp, "#%s Att.:1 NumFcs=%d (Patsolve Solution)%s", 
				gszCustomGameName, NumFcs, CRLF);
}
		fprintf (fpFile, szTemp);

		for (i = gnFCSolveIndex, j = 0; i >= 0; i -= 2)
		{
		if (FCSolveMoves[i] != 'x')
			{
			fprintf (fpFile, "%c%C ", 
						FCSolveMoves[i], FCSolveMoves[i - 1]);
			if ((++j % STD_MOVES_PER_LINE) == 0)
				{
				fprintf (fpFile, "\n");
				}
			}
		}
		SaveFlag1 = SaveFlag2 = SaveFlag3 = FALSE ;
		fclose (fpFile);
	}
	// add 'Solved' to top line of log file
	gbSolver2 = PlayMode2 = gbSolver3 = FALSE ;
	gbSolverLoaded = TRUE;
	UpdateLogFile (FALSE);
}

VOID FCSolveReadOriginal ()
{
	BYTE yCol, yPos;
	Card cd;
	Column *col;
#ifdef PRESORT
	INT ColLow[MAXCOL - 1] ;
	INT ColLCnt[MAXCOL - 1] ;
	INT ColLPos[MAXCOL - 1] ;
	INT i, thru, need, temp ;
//char szDiagTx[20] ;
#endif
	// clear orig
//MainMessage (PresortC);

	for (yCol = 0; yCol < NUM_COLS; yCol++)
	{
		orig.tableau[yCol].count = 0;
	}

	for (yPos = 0; yPos < NUM_FCS; yPos++)
	{
		orig.hold[yPos] = 0;
	}

	for (yPos = NUM_FCS; yPos < (NUM_FCS + NUM_SUITS); yPos++)
	{
		orig.foundations[yPos-NUM_FCS] = 0;
	}

	// load orig

	yCol = 0;

	for (yPos = 0; yPos < NUM_FCS; yPos++)
	{
		if (card[yCol][yPos] != EMPTY)
		{
			cd = VALUE(card[yCol][yPos])+1;
			switch (SUIT(card[yCol][yPos]))
			{
				case CLUB:
					cd += CLUBS;
					break;
				case DIAMOND:
					cd += DIAMONDS;
					break;
				case HEART:
					cd += HEARTS;
					break;
				case SPADE:
					cd += SPADES;
					break;
			}
		orig.hold[yPos] = (Pair) cd;
		}
	}

	for (yPos = NUM_FCS; yPos < (NUM_FCS + NUM_SUITS); yPos++)
	{
		if (card[yCol][yPos] != EMPTY)
		{
			cd = VALUE(card[yCol][yPos])+1;
			switch (SUIT(card[yCol][yPos]))
			{
				case CLUB:
					orig.foundations[0] = (uchar) cd;
					break;
				case DIAMOND:
					orig.foundations[1] = (uchar) cd;
					break;
				case SPADE:
					orig.foundations[2] = (uchar) cd;
					break;
				case HEART:
					orig.foundations[3] = (uchar) cd;
					break;
				default:
					MainMessage ("Unknown suit.");
			}
		}
	}

//#ifndef PRESORT	
for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
			ColSeq[yCol] = yCol + 1 ;
if (!PresortC[0])
{
	for (yPos = 0; yPos < MAXPOS; yPos++)
	{
		for (yCol = 1; yCol < MAXCOL; yCol++)
		{
			if (card[yCol][yPos] != EMPTY)
			{
				cd = VALUE(card[yCol][yPos])+1;
				switch (SUIT(card[yCol][yPos]))
				{
					case CLUB:
						cd += CLUBS;
						break;
					case DIAMOND:
						cd += DIAMONDS;
						break;
					case HEART:
						cd += HEARTS;
						break;
					case SPADE:
						cd += SPADES;
						break;
				}
				col = &orig.tableau[yCol-1];
				col->cards[col->count++] = (Pair) cd;
			}
		}
	}
}
else
{
//#else
	//first pass to determine sequence
//for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
//			ColSeq[yCol] = yCol + 1 ;
if (PresortC[0] == 'C')
  {
	for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
	{
		ColLow[yCol] = EMPTY ;
		ColLCnt[yCol] = ColLPos[yCol] = 0 ;
	}
	for (yPos = 0; yPos < MAXPOS; yPos++)
	{
		for (yCol = 1; yCol < MAXCOL; yCol++)
		{
			if (card[yCol][yPos] != EMPTY)
			{
				cd = VALUE(card[yCol][yPos])+1;
				switch (SUIT(card[yCol][yPos]))
				{
					case CLUB:
						cd += CLUBS;
						break;
					case DIAMOND:
						cd += DIAMONDS;
						break;
					case HEART:
						cd += HEARTS;
						break;
					case SPADE:
						cd += SPADES;
						break;
				}
				if (VALUE(cd) == ColLow[yCol - 1])
				{
					ColLCnt[yCol - 1]++ ;
					ColLPos[yCol - 1] = yPos ;
				}
				if (VALUE(cd) < ColLow[yCol - 1])
				{
					ColLow[yCol - 1] = VALUE(cd) ;
					ColLCnt[yCol - 1] = 0 ;
					ColLPos[yCol - 1] = yPos ;
				}
				col = &orig.tableau[yCol-1];
				col->cards[col->count++] = (Pair) cd;
			}
		}
	}
	for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol ++)
	{
		col = &orig.tableau[yCol];
		ColLPos[yCol] = col->count - ColLPos[yCol] ;
	}
	thru = 7 ;

	while (thru > 0) 
	{
		i = need = 0;
		for (i=1; i<=thru; i++)
		{  
			if ((ColLow[ColSeq[i]] < ColLow[ColSeq[i - 1]]) ||
				((ColLow[ColSeq[i]] == ColLow[ColSeq[i - 1]]) &&
					(ColLCnt[ColSeq[i]] > ColLCnt[ColSeq[i -1]])) ||
				((ColLow[ColSeq[i]] == ColLow[ColSeq[i - 1]]) &&
					(ColLCnt[ColSeq[i]] == ColLCnt[ColSeq[i -1]]) &&
					(ColLPos[ColSeq[i]] < ColLPos[ColSeq[i -1]])))				
			{
				temp = ColSeq[i];
				ColSeq[i] = ColSeq[i-1];
				ColSeq[i-1] = temp ;
				need = i-1;
			}
		}
		thru = need;
	}
  }
else
  {
  if (PresortC[0] == 'A')
	{
	for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
		ColSeq[yCol] = MAXCOL - 1 - yCol ;
    }
  if (PresortC[0] == 'B')
    {
	  for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
	    {
		if (yCol < 4)
		  ColSeq[yCol] = yCol + 5 ;
		else
		  ColSeq[yCol] = yCol - 3 ;
	    }
    }
  if (PresortC[0] == 'D')
	{
	ColSeq[0] = 8 ;
    ColSeq[7] = 1 ;
    }
  if (PresortC[0] == 'E')
	{
	ColSeq[0] = 7 ;
	ColSeq[1] = 8 ;
	ColSeq[6] = 1 ;
	ColSeq[7] = 2 ;
    }
  if (PresortC[0] == 'F')
	{
	ColSeq[0] = 6 ;
	ColSeq[1] = 7 ;
	ColSeq[2] = 8 ;
	ColSeq[5] = 1 ;
	ColSeq[6] = 2 ;
	ColSeq[7] = 3 ;
    }
  if (PresortC[0] == 'G')
	{
	for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
	  {
		ColSeq[yCol] = (yCol + 2)%8 ;
		if (!ColSeq[yCol])
			ColSeq[yCol] = 8 ;
	  }
	}
  if (PresortC[0] == 'H')
	{
 	for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
	  {
		ColSeq[yCol] = (yCol + 3)%8 ;
		if (!ColSeq[yCol])
			ColSeq[yCol] = 8 ;
	  }
    }
  if (PresortC[0] == 'I')
	{
	for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
	  {
		ColSeq[yCol] = (yCol + 4)%8 ;
		if (!ColSeq[yCol])
			ColSeq[yCol] = 8 ;
	  }
	}
  if (PresortC[0] == 'J')
	{
	for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
	  {
		ColSeq[yCol] = (yCol + 6)%8 ;
		if (!ColSeq[yCol])
			ColSeq[yCol] = 8 ;
	  }
    }
  if (PresortC[0] == 'K')
	{
	for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
	  {
		ColSeq[yCol] = (yCol + 7)%8 ;
		if (!ColSeq[yCol])
			ColSeq[yCol] = 8 ;
	  }
    }
  if (PresortC[0] == 'L')
	{
	for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
	  {
		ColSeq[yCol] = (yCol + 7)%8 ;
		if (!ColSeq[yCol])
			ColSeq[yCol] = 8 ;
	  }
	}
  if (PresortC[0] == 'M')
	{
	for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
	  {
		ColSeq[yCol] = (9 - yCol)%8 ;
		if (!ColSeq[yCol])
			ColSeq[yCol] = 8 ;
	  }
	}
  if (PresortC[0] == 'N')
	{
	for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
	  {
		ColSeq[yCol] = (10 - yCol)%8 ;
		if (!ColSeq[yCol])
			ColSeq[yCol] = 8 ;
	  }
	}
  if (PresortC[0] == 'O')
	{
	for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
	  {
		ColSeq[yCol] = (11 - yCol)%8 ;
		if (!ColSeq[yCol])
			ColSeq[yCol] = 8 ;
	  }
	}
  if (PresortC[0] == 'P')
	{
	for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
	  {
		ColSeq[yCol] = (12 - yCol)%8 ;
		if (!ColSeq[yCol])
			ColSeq[yCol] = 8 ;
	  }
	}
  if (PresortC[0] == 'Q')
	{
	for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
	  {
		ColSeq[yCol] = (13 - yCol)%8 ;
		if (!ColSeq[yCol])
			ColSeq[yCol] = 8 ;
	  }
	}
  if (PresortC[0] == 'R')
	{
	for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
	  {
		ColSeq[yCol] = (14 - yCol)%8 ;
		if (!ColSeq[yCol])
			ColSeq[yCol] = 8 ;
	  }
	}
  if (PresortC[0] == 'S')
	{
	for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
	  {
		ColSeq[yCol] = (15 - yCol)%8 ;
		if (!ColSeq[yCol])
			ColSeq[yCol] = 8 ;
	  }
	}
//  if ((PresortC[0] == 'V') || (PresortC[0] == 'U'))
//	{
//	if (!gnExtendTry)
//	    {
//		for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
//			if (yCol < (PresortC[1] - '0'))
//			  ColSeq[yCol] = (PresortC[1] - '0') - yCol ;
//			else
//			  ColSeq[yCol] = yCol + 1 ;
//		gnExtendTry = 1 ;
//		}
//	}
  }
// now load again in column sequence
	for (yCol = 0; yCol < NUM_COLS; yCol++)
	{
		orig.tableau[yCol].count = 0;
	}

	for (yPos = 0; yPos < MAXPOS; yPos++)
	{
		for (yCol = 1; yCol < MAXCOL; yCol++)
		{
			if (card[yCol][yPos] != EMPTY)
			{
				cd = VALUE(card[yCol][yPos])+1;
				switch (SUIT(card[yCol][yPos]))
				{
					case CLUB:
						cd += CLUBS;
						break;
					case DIAMOND:
						cd += DIAMONDS;
						break;
					case HEART:
						cd += HEARTS;
						break;
					case SPADE:
						cd += SPADES;
						break;
				}
				col = &orig.tableau[ColSeq[yCol- 1] - 1];
				col->cards[col->count++] = (Pair) cd;
			}
		}
	}
//for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
//		ColSeqC[yCol] = (char)ColSeq[yCol] + '0' ;
}
for (yCol = 0 ; yCol < MAXCOL - 1 ; yCol++)
		ColSeqC[yCol] = (char)ColSeq[yCol] + '0' ;
//#endif
	FCSolveInit ();
}					  
