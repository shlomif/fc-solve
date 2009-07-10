// 1997, wilson callan & adrian ettlinger, 1998

#define SSMOVE 1
#define SSMOVEP 1
#define SSMOVEDIAG 1

#ifdef FCPRO2

#include "main.h"
#include "settings.h"
#include "freecell.h"
#include "log.h"
#include "fcplay.h"
#include "fcsolve.h"

#else

#include <windows.h>
#include "fcpro.h"
#if !DBG
#define PRIVATE static
#else
#define PRIVATE
#endif

#endif

#ifdef DIAG
INT ErrCardS, ErrCardL, ErrCardD, ErrColLenS, ErrColLenD, ErrCode;
#endif

#define NUM_STORAGE		(50)

#ifdef SSMOVEDIAG
char szDiagTx[50] ;
#endif

INT Fvac, Cvac, FvacO ;
INT SingleFlag;
INT FcMode, MultMove, MultMoveS ;
#ifdef SSMOVE
INT MultMoveX, MultMoveXS, smcnt, sccnt, SMLim ;
char SSMScript[60] ;
char Osrc ;
#endif
INT NumFcs = 4 ;
INT SupMove = 1 ;
BOOL PossMoves[FCPLAYsizePROAID_POSS_MOVES];
BYTE posS[NUM_COLS][FCPLAYstatesPROLINE]; // 0-3
extern INT gnUserMoves ;
extern BOOL gbMoveID ;
extern BOOL PlayMode2 ;
PRIVATE STD_MOVES gStorage[NUM_STORAGE];
PRIVATE INT  gnStorages;
PRIVATE CHAR dcolc;
PRIVATE INT mcurcol, mupind, mmcnt ;
PRIVATE INT ColUsed[8];
PRIVATE INT Fvac_max;

PRIVATE BOOL MakeMove (CHAR source, CHAR destination, INT mode);
PRIVATE BOOL MoveColToFullCol (INT Scol, INT Dcol, INT mode);
PRIVATE BOOL MoveColToEmptyCol (INT Scol, INT Dcol, INT mode);
PRIVATE BOOL MoveColToFC (INT SCol, INT dst, INT mode);
PRIVATE BOOL MoveColToHome (INT Scol, INT dst, INT mode);
PRIVATE BOOL MoveFCToCol (CARD Scard, INT dst, INT trpos, INT mode);
PRIVATE BOOL MoveFCToFCOrHome (CARD Scard, INT dst, INT trpos, INT mode);
PRIVATE CHAR MoveCard (INT col, INT pos, CARD crd, BOOL bTestMode, BOOL bAutoMove);
PRIVATE VOID StoreCard (INT Scol, INT Spos, CARD Scard);
PRIVATE VOID UnstoreCards (INT Dcol);

int HSSignal ;
int cdMoveCnt ;
BOOL cdMoveCnt2 ;
BOOL Spec46 ;

char szTemp[50] ;

VOID FCPlayInit (VOID)
{
	Cvac = 0;
	SingleFlag = 0;
	FcMode = 1;
	Fvac = Fvac_max = NumFcs;
}

VOID FCPlayReInit (HWND hwnd)
{
	BYTE yCol, yOldPos, yNewPos;
	if (Fvac_max > NumFcs)
	{
		// ie going from 4 to 2, subtract 2

		//
		// move homecells down
		//

		yCol = 0;
		// check fcs are empty

		for (yNewPos = NumFcs; yNewPos < (NumFcs + NUM_SUITS); yNewPos++)
		{
			if (yNewPos >= Fvac_max)
			{
				// have checked all freecells
				break;
			}
			else if (card[yCol][yNewPos] != EMPTY)											{
				// freecell is not empty!
				
				Message (hwnd, MB_ICONEXCLAMATION, 
					"Cannot reduce freecells from %d to %d\nbecause freecell %d is not empty!",
					Fvac_max, NumFcs, yNewPos+1);
				NumFcs = Fvac_max;
				return ;
			}
		}

		// move cards

		for (yOldPos = Fvac_max, yNewPos = NumFcs; 
			 yOldPos < (Fvac_max + NUM_SUITS); yOldPos++, yNewPos++)
		{
			card[yCol][yNewPos] = card[yCol][yOldPos];
			card[yCol][yOldPos] = EMPTY;
		}

		// adjust Freecell vacancies

		Fvac -= (Fvac_max - NumFcs);

		if (Fvac < 0) Fvac = 0;
	}
	else
	{
		// ie going from 2 to 4, add 2

		//
		// move homecells up 
		//

		yCol = 0;
		for (yOldPos = (Fvac_max + NUM_SUITS), 
			 yNewPos = (NumFcs   + NUM_SUITS -1); 
			 yOldPos >= Fvac_max + 1; yOldPos--, yNewPos--)
		{
			card[yCol][yNewPos] = card[yCol][yOldPos - 1];
			card[yCol][yOldPos -1] = EMPTY;
		}
		// adjust Freecell vacancies

		Fvac += (NumFcs - Fvac_max);
	}

	// store new max size

	Fvac_max = NumFcs;
}

VOID EnableSupermove (VOID)
{
	SupMove = 1 ;
}

VOID DisableSupermove (VOID)
{
	SupMove = 0 ;
}

VOID EnableProAids (VOID)
{
	gbProAids = TRUE ;
}

VOID DisableProAids (VOID)
{
	gbProAids = FALSE ;
}

VOID ActivateSupMoveTest (VOID)
{
//MainMessage("ASMT") ;
	if (SupMove)
//	{
//	MainMessage("ASMT2") ;
	  FcMode = 0 ;
//	}
}

BOOL GDone (VOID)
{
	BYTE yPos, yCol ;
	// check if game won! 
	yCol = 0 ;
	for (yPos = NUM_FCS; yPos < (NUM_FCS + NUM_SUITS); yPos++)
	{
		if (VALUE(card[yCol][yPos]) != KING)
			// cant win without king in homecell
			return FALSE ;
	}
	return TRUE ;
}

BOOL MakeMoveH (CHAR source, CHAR destination)
{
	INT  ColLenT[8];
	BYTE i;
	CHAR Tchar, Tchofs;
	BOOL result;

	if (GDone())
	{
		return FALSE ;
	}
//	if (destination < 'e') 
//#ifdef FC89
#ifndef FC89
	if (destination < ('a' + NumFcs))    //TEST 
#else
//e:		if ((destination < ('a' + NumFcs)) || ((NumFcs > 7) && (destination >= 'k') && (destination <= 'l') &&
		if (((destination < ('a' + NumFcs)) || (destination >= 'k') || (destination <= 'l')) &&
				(destination != 'h'))//TEST 
//		if ((destination < ('a' + NumFcs)) || ((NumFcs > 7) && (destination <= 'l') &&
//				(destination != 'h')))//TEST 
//		if (destination != 'h')
#endif
				// destination is not home
		result = MakeMove(source, destination, 0) ;
	else
	{
		// make a copy of ColLen[]
#ifndef NTMAKE
		_fmemcpy(ColLenT, ColLen, 16) ;
#else
		for (i = 0 ; i < 8 ; i++)
			ColLenT[i] = ColLen[i] ;
#endif
		// test to see if can move into any homecell
//		for (Tchar = 'e' ; Tchar < 'i' ; Tchar++)
#ifndef FC89
	for (Tchar = ('a' + NumFcs) ; Tchar < ('e' + NumFcs) ; Tchar++)  // this tests home positions
#else
	Tchofs = 0 ;
	if (NumFcs > 7)
		Tchofs = 3 ;
	for (Tchar = ('a' + NumFcs + Tchofs) ; Tchar < ('e' + NumFcs + Tchofs) ; Tchar++)  // this tests home positions
#endif
		{
			if (MakeMove (source, Tchar, 1))
				{
				break ;
				}
		}
		// restore ColLen[]
#ifndef NTMAKE
		_fmemcpy(ColLen, ColLenT, 16) ;
#else
		for (i = 0 ; i < 8 ; i++)
			ColLen[i] = ColLenT[i] ;
#endif
//		if (Tchar < 'i')
#ifndef FC89
		if (Tchar < ('e' +NumFcs))
#else
		if (Tchar < ('e' +NumFcs+3))
#endif
			// can make move, so do it
		{
			result = MakeMove(source, Tchar, 0) ;
		}
		else
			result = FALSE ;
	}
  return result ;
}

BOOL MakeMoveT (CHAR source, CHAR destination)
{
	INT  ColLenS[8];
	CHAR Tchar;
	BOOL result;
	BYTE i;
	char Tchofs ;
	//char szTemp[20] ;
#ifndef NTMAKE

	_fmemcpy(ColLenS, ColLen, 16) ; 

#else
//	MainMessage("MMTEnt");
//sprintf(szTemp, "MMTE src-%c dst-%c", source, destination) ;
//MainMessage(szTemp) ;
	for (i = 0 ; i < 8 ; i++)
	{
	  ColLenS[i] = ColLen[i] ;
	}

#endif
#ifndef FC89
	if (destination < ('e' + NumFcs - 4))
#else
		if (((NumFcs < 8) && (destination < ('e' + NumFcs - 4))) || ((NumFcs > 7) && ((destination < 'h') ||
		((destination >= 'k') && (destination <= 'l')))))
#endif
	{
		result = MakeMove(source, destination, 1) ;
//sprintf(szTemp, "MMT0 res-%d des-%c", result, destination) ;
//MainMessage(szTemp) ;
	}
	else
	{
#ifndef FC89
	for (Tchar = ('a' + NumFcs) ; Tchar < ('e' + NumFcs) ; Tchar++)  // this tests home positions
#else
	Tchofs = 0 ;
	if (NumFcs > 7)
		Tchofs = 3 ;
	for (Tchar = ('a' + NumFcs + Tchofs) ; Tchar < ('e' + NumFcs + Tchofs) ; Tchar++)  // this tests home positions
#endif
	{
//sprintf(szTemp, "MMT Tc-%c ", Tchar) ;
//MainMessage(szTemp) ;
		if (MakeMove(source, Tchar, 1))
	      break ;
    }
#ifndef FC89
	if (Tchar < ('e' + NumFcs))
#else
	if (Tchar < ('e' + NumFcs + Tchofs))
#endif
	{
//MainMessage("MMT Tex") ;
		result = TRUE ;
	}
	else
	  {
//MainMessage("MMT Fex") ;
	    result = FALSE ;
	  }
		//	  result = MakeMove(source, destination, 1) ;
	}
#ifndef NTMAKE
	_fmemcpy(ColLen, ColLenS, 16) ; 
#else
	for (i = 0 ; i < 8 ; i++)
	  ColLen[i] = ColLenS[i] ;
#endif
return result ;
}
BOOL MakeMoveTP (CHAR source, CHAR destination)
{
	INT  ColLenS[8];
	BOOL result;
	BYTE i;
#ifndef NTMAKE
	_fmemcpy(ColLenS, ColLen, 16) ; 
#else
	for (i = 0 ; i < 8 ; i++)
	{
	  ColLenS[i] = ColLen[i] ;
	}
#endif
	result = MakeMove(source, destination, 1) ;
#ifndef NTMAKE
	_fmemcpy(ColLen, ColLenS, 16) ; 
#else
	for (i = 0 ; i < 8 ; i++)
	  ColLen[i] = ColLenS[i] ;
#endif
return result ;
}


PRIVATE BOOL MakeMove (CHAR source, CHAR destination, INT mode)
{
	INT src, dst ;
	INT Scol, Dcol, trpos;
	CARD Scard;
	BOOL rtn = TRUE;
	src = (INT) source;
	dst = (INT) destination;
//MainMessage("MMEnt") ;
	MultMove = 0 ;
	if (isdigit(src))
	{		// Move from column
		Scol = src - '0' ;

		if (!ColLen[Scol - 1])
		{
			// attempt to move from empty column
			rtn = FALSE;
		}
		else
		{
			if (isdigit(dst))
			{   // Move from column to column
				Dcol = dst - '0' ;

				if (ColLen[Dcol -1])
				{  // Move from column to non-empty column
					rtn = MoveColToFullCol (Scol, Dcol, mode);
				}
				else
				{   // move from column to empty column
					rtn = MoveColToEmptyCol (Scol, Dcol, mode);
  }
			}
			else
			{		//Move from column to freecell or home
#ifdef FCPRO2
//wcaldif				
				Scard = card[Scol][ColLen[Scol - 1] - 1] ;
//sprintf(szTemp,"MM2 sc-%d ", Scard);
//MainMessage(szTemp) ;
				ShowCardG (Scard, TRUE) ;
#endif
//				if (dst < 'e')
//				if (dst < 'h')  //TEST
//#ifdef FC89
#ifndef FC89
				if (dst < ('a' + NumFcs))
#else
				if (((NumFcs < 8) && (dst < ('a' + NumFcs))) || 
					((NumFcs > 7) && ((dst < 'h') || (dst == 'k'))) || 
					((NumFcs == 9) && (dst == 'l')))
//orig:				if ((dst < ('a' + NumFcs)) || ((dst >= 'k') && (dst <= 'l'))) //TEST
#endif
				{	// Move from column to freecell
#ifdef FCPRO2
//wcaldif					
					ShowCell ("FC");
#endif
//sprintf(szTemp, "MM MCtF ND-%d ds-%c", NumFcs, dst) ;
//MainMessage(szTemp) ;
					rtn = MoveColToFC (Scol, dst, mode);
				}
				else
				{   // Move from column to home
#ifdef FCPRO2
//wcaldif					
					ShowCell ("HC");
#endif
//MainMessage("MM MCtH") ;
					rtn = MoveColToHome (Scol, dst, mode);
				}
			}
	        if ((!ColLen[Scol -1]) && (!mode))
			{
				Cvac++;
			}
		}
	}
	else
	{  //Move from freecell or home
//		if (src > 'd')
//sprintf(szTemp, "FtH1 sc-%c ", src) ;
//MainMessage(szTemp) ;
#ifndef FC89
		if (src > ('a' + NumFcs - 1))
				rtn = FALSE;
#else
		if (((src > ('a' + NumFcs - 1)) && (NumFcs < 8)) || 
				(((src > ('a' + NumFcs + 2)) && (NumFcs >= 8))))  
		{
			if (src == 'h')
			//attempt to move from home
				rtn = FALSE;
		}
#endif
		else
	    {  //Move from freecell
			trpos = src - 'a' ;
#ifdef FC89
			if (trpos > 6)
				trpos -= 3 ;
#endif
			Scard = card[0][trpos];
//sprintf(szTemp,"MFF sc-%c", Scard) ;
//MainMessage(szTemp) ;
#ifdef FCPRO2
//wcaldif			
			ShowCardG(Scard, TRUE);
#endif
			if (Scard == EMPTY)
			{
				// attempt to move from empty freecell
				rtn = FALSE;
			}
			else
			{
				if (isdigit(dst))
				{ // Move from freecell to column

					rtn = MoveFCToCol (Scard, dst, trpos, mode);
				}
				else
				{ // Move from freecell to freecell or home
					rtn = MoveFCToFCOrHome (Scard, dst, trpos, mode);
//sprintf(szTemp,"MFtH rtn-%d", rtn) ;
//MainMessage(szTemp) ;
				}
			}
		}
	}

	if (!mode)
	{
		// test mode first
		FlourishCheck ();
if (!gbSolver2 && !PlayMode2 && (cdMoveCnt == 0))
		// do the real thing
	MoveAuto (FALSE);
	}

	return rtn;
}

// ************************************
// Move from column to non-empty column
//
PRIVATE BOOL MoveColToFullCol (INT Scol, INT Dcol, INT mode)
{
	CARD Scard, Dcard, LScard;
#ifndef SSMOVE
	INT  i, j; 
#else
	INT  i, j, k ;	  
	CARD SScard ;
#endif	
	BOOL rtn = TRUE;
//if (Scol == 6)
//MainMessage("CtCEnt") ;
	
	// get dst card val from array
	Dcard = card[Dcol][ColLen[Dcol - 1] - 1] ;

#ifdef FCPRO2
	ShowCardG(Dcard, FALSE) ;
#endif

	// test from 0 to most cards we can move,
	//		fcVacant+1 * colVacant+1
#ifndef SSMOVE
	for (i = 0; i < (Fvac + 1) * (Cvac + 1) ; i++)
#else
	if ((((FcMode != 0) && !gbSolver2 && !PlayMode2) || (FcMode == 2)) && (cdMoveCnt == 0))
		k = (Fvac + 1) * (Cvac + 1) ;
    else
	{
		// SuperMove enabled
		if (PlayMode2 && (cdMoveCnt != 0))
		{
			FcMode = 0 ;
			PlayMode2 = FALSE ;
			cdMoveCnt2 = TRUE ;
		}
		i = Cvac ;
		if (i > 4)
		  i = 4 ;
		k = (1<<i) * (Fvac + 1) ;
// Test how many cards could be moved
	for (i = 0 ; i < k ; i++)
		{
		Scard = card[Scol][ColLen[Scol - 1] - 1 - i] ;
		if (!i)
			SScard = Scard ;
		if ((i && (((VALUE(Scard) - 1) != VALUE(LScard)) ||
			(COLOUR(Scard) == COLOUR(LScard)))) ||
			(i >= ColLen[Scol -1]))
	      break ;
		LScard = Scard ;
		}
	if (i >	(Fvac + 1) * (Cvac + 1))
		{
		SMLim = 0 ;
		if (i > (VALUE(Dcard) - VALUE(SScard)))
			{
			j = i ;
			i = VALUE(Dcard) - VALUE(SScard) ;
			if (i < j)
			  SMLim = j - i ;
			k = i ;
			}
		if (i > (Fvac + 1) * (Cvac + 1))
			MultMove = MultMoveX = i ;		
		}
	}
	for (i = 0; i < k ; i++)
#endif
	{
		// get src card val from array

		Scard = card[Scol][ColLen[Scol - 1] - 1 - i] ;

		if ((i && (((VALUE(Scard) - 1) != VALUE(LScard)) ||
			(COLOUR(Scard) == COLOUR(LScard)))) ||
			(i >= ColLen[Scol -1]))
		{
			// cannot move any cards of column

#ifdef DIAG
		  ErrCardS= Scard ;
		  ErrCardD = Dcard ;
		  ErrCardL = LScard ;
		  ErrColLenS = ColLen[Scol - 1] ;
		  ErrColLenD = ColLen[Dcol - 1] ;
		  ErrCode = 1 ;
#endif
		  rtn = FALSE;
			break;
		}

#ifdef FCPRO2
		// show this card, tho it might get overwritten
		ShowCardG(Scard, TRUE) ;
#endif
		if ((VALUE(Scard) + 1) == VALUE(Dcard)  &&
			(COLOUR(Scard) != COLOUR(Dcard)))
		{
			// move all cards in column

			gnStorages = 0;

			for (j = 0; j <= i; j++)
			{
				// src card is now top card
				Scard = card[Scol][ColLen[Scol - 1] - 1] ;

				// change hand - move cards to fcs and empties
				// as we need

				if (j == i)
				{
					// its the last card so move to column
//					if (i <= t)
//				         MultMove = MultMoveX = 0 ;
					if (!mode)
					{
						MoveCard (Scol, ColLen[Scol - 1] - 1, EMPTY, FALSE, FALSE);
						MoveCard (Dcol, ColLen[Dcol - 1], Scard, FALSE, FALSE);
					}

					// dst column is now bigger
					ColLen[Dcol - 1]++;
					// src column is now smaller
					ColLen[Scol - 1]--;
				}
				else
				{
					// its not the last card so put in storage

					// in the move col to full col case, all vacant
					// freecells and empty columns are used to store
					// cards.  this includes stacking cards up into
					// empty columns.

					if (!mode)
					{
					StoreCard (Scol, ColLen[Scol - 1] - 1, Scard);
					}

					// src column is now smaller
					ColLen[Scol - 1]--;
				}
			}

			// if we put cards in storage, then play them out backwards

			if (!mode)
			{
				// note that collen[] is changed in unstorecards ()

				UnstoreCards (Dcol);
			}
//			MultMove = MultMoveX = 0 ;
//			if (i > (Fvac + 1) * (Cvac + 1))
//				MultMove = MultMoveX = i ;
//			i = (NUM_FCS+1)*(NUM_COLS+1) + 2; // exit code
			i = 1000 ;
		}

		// save for next loop thru

		LScard = Scard ;
	}

//	if (i < (NUM_FCS+1)*(NUM_COLS+1) + 2)
	if (i < 1000)	// nasty crash bug from supersupermove
	{
		// not exit code, didn't find a match
#ifdef DIAG
		  ErrCardS= Scard ;
		  ErrCardD = Dcard ;
		  ErrCardL = LScard ;
		  ErrColLenS = ColLen[Scol - 1] ;
		  ErrColLenD = ColLen[Dcol - 1] ;
		  ErrCode = 2 ;
#endif
//if (Scol == 6)
//MainMessage("CtCEx2") ;
		rtn = FALSE;
	}
	return rtn;
}

//********************************
// move from column to empty column
//
PRIVATE BOOL MoveColToEmptyCol (
INT Scol, 
INT Dcol,INT mode)
{
	INT i, j, k;
	CARD Scard, LScard;
	BOOL rtn = TRUE;

	SMLim = 0 ;  // Should be cleared here -- could be left over from a CtoFC move  1/4/02
	j = 1 ;
//	if (FcMode)
	if (((FcMode) && (cdMoveCnt == 0)) || cdMoveCnt2)
	{
		k = Fvac + 1 ;
	}
    else
	{
		// SuperMove enabled
#ifndef SSMOVE
		k = Cvac * (Fvac + 1) ;
#else
		if (Cvac < 3)
			k = Cvac * (Fvac + 1) ;
		else
			{
			i = Cvac - 1;
			if (i > 4)
				i =	4 ;
			k = (1<<i) * (Fvac + 1) ;
			}
#endif
	}
//	if (gbSolver3)
//		k = 1 ;   // if in Solver 3, override and move one card only
	// test from 0 to most cards we can move

	if (PlayMode2 && (cdMoveCnt != 0))
	{
		FcMode = 0 ;
		PlayMode2 = FALSE ;
		cdMoveCnt2 = TRUE ;
//		k = cdMoveCnt ;	//Override for solve2
	}
    for (i = 0; i < k ; i++)
	{
		// get src card val from array

		Scard = card[Scol][ColLen[Scol - 1] - 1 - i] ;
#ifdef FCPRO2
		ShowCardG(Scard, TRUE) ;
#endif
		if (i && 
			(
				(
					((VALUE(Scard) - 1) != VALUE(LScard)) ||
					(COLOUR(Scard) == COLOUR(LScard))
				) ||
				(i >= ColLen[Scol -1]) || SingleFlag
			)
		   )
		{
			j = i ;	  // this is our new value
//			i = 40 ;  // exit loop
			i = 1000 ;	// nasty crash bug fix
		}

		// save for next loop thru
		LScard = Scard ;
	}

    if (i == k)
	{
		// loop went all the way thru
		j = i ;
	}

	i = j ;
	MultMove = 0 ;
#ifdef SSMOVE
	MultMoveX = 0 ;
#endif
//	if ((!FcMode || gbSolver2) && (j > (Fvac + 1)) && (FcMode != 2))
	if ((!FcMode || (PlayMode2 && (cdMoveCnt != 0))) && (j > (Fvac + 1)))
	{
		// super move is enabled
		MultMove = j ;
#ifdef SSMOVE
	if (j > Cvac * (Fvac + 1))
		MultMoveX = j ;
#endif
	}
	gnStorages = 0;
	for (j = 0 ; j < i ; j++)
	{
		// get src card val from array

		Scard = card[Scol][ColLen[Scol - 1] - 1] ;

		// change hand - move cards to fcs and empties as we 
		// need

		if (j == (i - 1))
		{
			if (!mode)
			{
				MoveCard (Scol, ColLen[Scol - 1] - 1, EMPTY, FALSE, FALSE);
				MoveCard (Dcol, ColLen[Dcol - 1], Scard, FALSE, FALSE);
			}

			// dst column is now bigger

			ColLen[Dcol - 1]++ ;

			// src column is now smaller

			ColLen[Scol - 1]--;
		}
		else
		{
			// its not the last card so put in storage

			// note that empty columns should not be use here
			// but since it wont be necessary to store move than 
			// the number of freecells, it wont happen

			if (!mode)
			{
			StoreCard (Scol, ColLen[Scol - 1] - 1, Scard);
			}

			// src column is now smaller

			ColLen[Scol - 1]--;
		}
	}

	// if we put cards in storage, then play them out backwards

	if (!mode)
	{
		// note that collen[] is changed in unstorecards ()

		UnstoreCards (Dcol);
	}

    if (!mode)
	{
		// less vacant columns
		Cvac-- ;
	}

	return rtn;
}


	// Move from column to freecell
PRIVATE BOOL MoveColToFC (INT Scol, INT dst, INT mode)
{
	CARD Scard;
	INT trpos;
	BOOL rtn = TRUE;

	trpos = dst - 'a' ;
#ifdef FC89
	if (trpos > 6)
		trpos -= 3 ;
#endif
	Scard = card[Scol][ColLen[Scol - 1] - 1] ;

	if (card[0][trpos] != EMPTY)
	{
		rtn = FALSE;
	}
	else
	{
		// change hand
		if (!mode)
		{
			MoveCard (Scol, ColLen[Scol - 1] - 1, EMPTY, FALSE, FALSE);
			MoveCard (0, trpos, Scard, FALSE, FALSE);

			// move col to fc, so one less vacant fc
//MainMessage("CtF") ;
			Fvac--;
		}
  		ColLen[Scol - 1]-- ;
	}

	return rtn;
}

	// Move from column to home
PRIVATE BOOL MoveColToHome (INT Scol, INT dst, INT mode)
{
	CARD Scard;
	INT dtrpos;
	BOOL rtn = TRUE;
//MainMessage("ColToHome") ;
	Scard = card[Scol][ColLen[Scol - 1] - 1] ;
	dtrpos = dst - 'a' ;
//sprintf(szTemp, "CtH dp-%d dst-%c ", dtrpos, dst);
//MainMessage(szTemp) ;
#ifdef FC89
	if ((dst >= 'k') && (NumFcs > 7))
		dtrpos-= 3 ;
#endif
	if ((VALUE(Scard) && 
		(((VALUE(card[0][dtrpos]) + 1) == VALUE(Scard)) &&
		(SUIT(card[0][dtrpos]) == SUIT(Scard)))) ||
		(!VALUE(Scard) && (card[0][dtrpos] == EMPTY)))
	{
		// change hand
		if (!mode)
		{
			MoveCard (Scol, ColLen[Scol - 1] - 1, EMPTY, FALSE, FALSE);
			MoveCard (0, dtrpos, Scard, FALSE, FALSE);
		}
		ColLen[Scol - 1]-- ;
	}
	else
	{
		rtn = FALSE;
	}

	return rtn;
}

	// Move from freecell to column
PRIVATE BOOL MoveFCToCol (CARD Scard, INT dst, INT trpos, INT mode)
{
	INT Dcol;
	CARD Dcard;
	BOOL rtn = TRUE;

	Dcol = dst - '0' ;
	if (ColLen[Dcol -1])
	{  // Move from freecell to non-empty column
		Dcard = card[Dcol][ColLen[Dcol - 1] - 1] ;
#ifdef FCPRO2
//wcaldif		
		ShowCardG(Dcard, FALSE) ;
#endif
	}
	else
	{
		if (!mode)
		{
			Cvac-- ;
		}
	}

	if (((VALUE(Scard) + 1) == VALUE(Dcard) &&
		COLOUR(Scard) != COLOUR(Dcard)) || !ColLen[Dcol -1])
	{
		// change hand
		if (!mode)
		{
			MoveCard (0, trpos, EMPTY, FALSE, FALSE);
			MoveCard (Dcol, ColLen[Dcol - 1], Scard, FALSE, FALSE);

			// movefctocol so now 1 more vac fc

			Fvac++;
		}
		ColLen[Dcol -1] ++ ;
	}
	else
	{
		rtn = FALSE;
	}

	return rtn;
}

	// Move from freecell to freecell or home
PRIVATE BOOL MoveFCToFCOrHome (CARD Scard, INT dst, INT trpos, INT mode)
{
	INT dtrpos;
//char szTemp[30];
	BOOL rtn = TRUE;

	dtrpos = dst - 'a' ;
#ifdef FC89
	if ((dst >= 'k') && (NumFcs > 7))
		dtrpos-= 3 ;
#endif
	if ((dtrpos < NUM_FCS) && (dst != 'h'))
	{   // Move from freecell to freecell (legal though silly)

		// WCAL made this illegal so that undo and playback
		// dont have to support it
		{
//		MainMessage("FH Pt 1") ;
		rtn = FALSE;
		}
#if FC2FC
		if (card[0][dtrpos] != EMPTY)
		{
			rtn = FALSE;
		}
		else
		{
			// change hand
			if (!mode)
			{
				MoveCard (0, dtrpos, Scard, FALSE, FALSE);
				MoveCard (0, trpos, EMPTY, FALSE, FALSE);
			}
		}
#endif
	}
	else
	{   // Move from freecell to home
		if ((VALUE(Scard) && 
			(((VALUE(card[0][dtrpos]) + 1) == VALUE(Scard)) &&
			(SUIT(card[0][dtrpos]) == SUIT(Scard)))) ||
			(!VALUE(Scard) && (card[0][dtrpos] == EMPTY)))
		{
			// change hand
			if (!mode)
			{
				MoveCard (0, trpos, EMPTY, FALSE, FALSE);
				MoveCard (0, dtrpos, Scard, FALSE, FALSE);
				// move fc to home so now that fc is vac
				Fvac++ ;
			}
		}
		else
		{
	    rtn = FALSE;
		}
	}

	return rtn;
}

//Auto-posting
VOID MoveAuto (BOOL bTestMode)
{
	INT fini, i, j, k, l, post, b;
	CARD Scard;

//char	szTemp[50] ;
	if (gbSpider)
		return ;
    fini = 0 ;
    while (!fini)
	{
		// assume one more pass
		fini = 1 ;

		// check all 12 col/fcs
//		for (i = 0 ; i < 12 ; i++)
		for (i = 0 ; i < (8 + NUM_FCS) ; i++)
		{  
			//Auto-posting

			post = -1 ;

			if (i < 8)
			{
				// col
				Scard = card[i + 1][ColLen[i] - 1] ;
			}
			else
			{
				// fc
				Scard = card[0][i - 8] ;
			}

			// dont post empty card!
			if (Scard == EMPTY)
			{
				continue;
			}

			// examine homecells (positions 4-7 in row 0)
			for (j = NUM_FCS ; j < (NUM_FCS + 4) ; j++)
			{
				// if (card is ace and homecell is empty) or
				// (card is duece and homecell is ace and 
				// suits are equal)
				if ((!VALUE(Scard) && card[0][j] == EMPTY) ||
					((VALUE(Scard) == 1) && (!VALUE(card[0][j]) &&
					(SUIT(card[0][j]) == SUIT(Scard)))))
				{
					// post this card to this homecell
					post = j ;

					// stop looking at homecells
					//j = 8 ;
					break;
				}

				// if card is tres or more and 
				// homecell+1 equals this card and
				// they are the same suits
				if ((VALUE(Scard) > 1) && 
					(((VALUE(card[0][j]) + 1) == VALUE(Scard)) &&
					(SUIT(card[0][j]) == SUIT(Scard))))
				{
					// post this card to this homecell
					post = j ;
				    l = 0 ;
			
					// look at all homecells
//					for (k = 4 ; k < 8 ; k++)	
				for (k = NUM_FCS ; k < (NUM_FCS + 4) ; k++)
					{
						// if the homecell is not empty and
						// the hc color is not the card color and
						// the homecell is more than or equal to the
						//		card - 1
						if ((card[0][k] != EMPTY) &&
						   (COLOUR(card[0][k]) != COLOUR(Scard)) &&
						   (VALUE(card[0][k]) >= VALUE(Scard) - 1))
						{
							// hit
							l++ ;
						}
					}

					if (l != 2)
					{
						// we cannot post this card, not enough hits
						post = -1 ;

						// stop looking at homecells, since we already
						// found this card a homecell
						//j = k = 8 ;
						break ;
					}
				}
			}

			if (post != -1)
			{
				// we got a card to post! - change hand

				if (i < 8)
				{
					// card is from a col
//sprintf(szTemp, "s3- %d", gbSolver3) ;
//MainMessage(szTemp) ; 
					MoveCard (i + 1, ColLen[i] - 1, EMPTY, bTestMode, TRUE);
					if (!bTestMode && gbLoadingSolver && !gbSolver3)   //***12/19/01
					{
						for (b = gnFCSolveIndexC ; b > 0 ; b -=2)
						{
//						if ((FCSolveMoves[b] == (i + '1')) &&
						if ((FCSolveMoves[b] == (ColSeqC[i])) &&
							  (FCSolveMoves[b - 1] == 'h'))
							{	
								FCSolveMoves[b] = FCSolveMoves[b - 1] = 'x' ;
								break ;
							}
						}
					}

					// decrement column length, even in test 
					// mode so that we can see if this will 
					// end the game

					ColLen[i]-- ; 

					if (!ColLen[i])
					{
						// cvac is restored after test mode
						// cvac is looked at after test mode
						// completes

						Cvac++ ;
					}
				}
				else
				{
					// card is from a fc

					MoveCard (0, i - 8, EMPTY, bTestMode, TRUE);
					if (!bTestMode && gbLoadingSolver && !gbSolver3)
					{
						for (b = gnFCSolveIndexC ; b > 0 ; b -=2)
						{
							if ((FCSolveMoves[b] == (i - 8 + 'a')) &&
							  (FCSolveMoves[b - 1] == 'h'))
							{	
									FCSolveMoves[b] = FCSolveMoves[b - 1] = 'x' ;
									break ;
							}
						}
					}

					// auto move from fc to home, so fc is va

					// fvac is restored after test mode
					// fvac is looked at after test mode
					// completes
					Fvac++ ;
				if (Fvac > NumFcs)
					Fvac = NumFcs ;
				}

				MoveCard (0, post, Scard, bTestMode, TRUE);

				// we arent done here!
				fini = 0 ;
			}
		}
	}
}

PRIVATE CHAR MoveCard (
INT col, 
INT pos, 
CARD crd, 
BOOL bTestMode,
BOOL bAutoMove)
{
	// testmode is used in moveauto
	// the card array is restored after testing is done
//if (col==0)
//sprintf(szTemp, "MC cd-%x ps-%d", crd, pos);
//MainMessage(szTemp) ;
	card[col][pos] = crd;

#ifdef FCPRO2
	if (!bTestMode)
	{
		InvalidateCard (col, pos);

		// add move calls UpdateWindow ()

		return AddMove (col, pos, bAutoMove);
	}
	return 0; // BRUCE: ensures all control paths return a value
#endif
}

//
// determines if freecell or column has a card
//
// a modified version could be used as a more generic 
// EMPTY checker
BOOL ValidateClick (CHAR loc)
{
	BOOL rtn = TRUE;
	char lofs ;
	if (loc >= 'a')
	{
		// fc
		lofs = 0 ;
#ifdef FC89
		if (loc >= 'j')
			lofs = 3 ;
#endif
		if (card[0][loc - 'a' - lofs] == EMPTY)
		{
			rtn = FALSE;
		}
	}
	else
	{
		// col

		if (ColLen[loc - '1'] == 0)
		{
//MainMessage("Empty") ;
			rtn = FALSE;
		}
	}

	return rtn;
}

VOID MoveUndo (INT nSrcCol, INT nSrcPos, INT nDstCol, INT nDstPos)
{
	// error checking

	if (card[nDstCol][nDstPos] == EMPTY)
	{
		MainMessage ("Internal Error - cannot undo Empty!");
		return;
	}

	if (card[nSrcCol][nSrcPos] != EMPTY)
	{
		MainMessage ("Internal Error - cannot undo onto Full!");
		return;
	}

	// move card

	card[nSrcCol][nSrcPos] = card[nDstCol][nDstPos]; 

	InvalidateCard (nSrcCol, nSrcPos);

	if (nSrcCol == 0)  
	{
		// fc row

		if (nSrcPos < NUM_FCS)
		{
			// one less vacant fc

			Fvac--;
		}
	}
	else
	{
		if (++ColLen[nSrcCol - 1] == 1)
		{
			// one less vacant col

			Cvac--;
		}
	}

	// empty where it was

	if (nDstCol == 0)  // fc row
	{
		if (nDstPos >= NUM_FCS) 
		{
			if (VALUE(card[nDstCol][nDstPos]) > ACE)
			{
				// put one less card in home

				card[nDstCol][nDstPos] -= NUM_SUITS;
			}
			else
			{
				// put nothing in home

				card[nDstCol][nDstPos] = EMPTY;
			}
		}
		else 
		{
			// take card out of fc

			card[nDstCol][nDstPos] = EMPTY;

			// one more vacant fc
			Fvac++;
if (Fvac > NumFcs)
Fvac = NumFcs ;
		}
	}
	else
	{
		card[nDstCol][nDstPos] = EMPTY;

		if (--ColLen[nDstCol - 1] == 0)
		{
			// one more vacant col

			Cvac++;
		}
	}

	InvalidateCard (nDstCol, nDstPos);

	// show undo moves

	if (gnAnimation < ANIMATE_FLOURISH)
	{
		UpdateMain ();
	}
}

//
// find empty col or fc to store card in
//
PRIVATE VOID StoreCard (INT Scol, INT Spos, CARD Scard)
{
    BYTE yStgCol, yStgPos, yDstCol, yDstPos;
	CARD StgCard;
	INT  i, nStorages;

	// if last move in storage was to a column, we have to stack stored
	// freecell cards on to that column and then re use those freecells

	if (gnStorages && gStorage[gnStorages - 1].dst.column)
	{
		// last move was to a column, move all previous freecell cards to 
		// this column

		yDstCol = gStorage[gnStorages - 1].dst.column;
		yDstPos = gStorage[gnStorages - 1].dst.position; // should be 0!

		// get copy of gnStorages that we wont changes

		nStorages = gnStorages;

		for (i = 2; (nStorages - i) >= 0; i++)
		{
			// if this move was not to a freecell then we have found all
			// the freecell cards

			if (gStorage[nStorages - i].dst.column != 0)
			{
				break;
			}

			// move card from storage freecell to storage column

			yStgCol = gStorage[nStorages - i].dst.column; // should be 0!
			yStgPos = gStorage[nStorages - i].dst.position;

			StgCard = card[yStgCol][yStgPos];
			MoveCard (yStgCol, yStgPos, EMPTY, FALSE, TRUE);
			MoveCard (yDstCol, ++yDstPos, StgCard, FALSE, TRUE);

			gStorage[gnStorages].src.column   = yStgCol;
			gStorage[gnStorages].src.position = yStgPos;
			gStorage[gnStorages].dst.column   = yDstCol;
			gStorage[gnStorages].dst.position = yDstPos;

			if (++gnStorages >= (sizeof (gStorage) / sizeof (STD_MOVES)))
			{
				MainMessage ("Out of storage memory.");
			}
		}
	}
	
	// now that we have freed up the freecells, look for empty freecells
	// col 0 is top row

	yStgCol = 0;

    for (yStgPos = 0 ; yStgPos < NUM_FCS; yStgPos++)
	{
		if (card[yStgCol][yStgPos] == EMPTY)
		{
			MoveCard (Scol, Spos, EMPTY, FALSE, TRUE);
			MoveCard (yStgCol, yStgPos, Scard, FALSE, TRUE);

			gStorage[gnStorages].src.column   = Scol;
			gStorage[gnStorages].src.position = Spos;
			gStorage[gnStorages].dst.column   = yStgCol;
			gStorage[gnStorages].dst.position = yStgPos;

			if (++gnStorages >= (sizeof (gStorage) / sizeof (STD_MOVES)))
			{
				MainMessage ("Out of storage memory.");
			}
			return;
		}
	}

	// no empty fcs, check for empty columns
	// pos 0 tells us if its empty

	yStgPos = 0;

    for (yStgCol = 1; yStgCol < MAXCOL; yStgCol++)
	{
		if (card[yStgCol][yStgPos] == EMPTY)
		{
			MoveCard (Scol, Spos, EMPTY, FALSE, TRUE);
			MoveCard (yStgCol, yStgPos, Scard, FALSE, TRUE);

			gStorage[gnStorages].src.column   = Scol;
			gStorage[gnStorages].src.position = Spos;
			gStorage[gnStorages].dst.column   = yStgCol;
			gStorage[gnStorages].dst.position = yStgPos;
			if (++gnStorages >= (sizeof (gStorage) / sizeof (STD_MOVES)))
			{
				MainMessage ("Out of storage memory.");
			}
			return;
		}
	} 

	MainMessage ("Internal StoreCard () Error.");
}

PRIVATE VOID UnstoreCards (INT Dcol)
{
	INT  i;
	BYTE yStgCol, yStgPos, yDstCol, yDstPos;
	CARD Scard;

	// if we put cards in storage, then play them out backwards

	for (i = gnStorages; i > 0; i--)
	{
		// move from storage to destination or another storage

		yStgCol = gStorage[i - 1].dst.column;
		yStgPos = gStorage[i - 1].dst.position;

		if (gStorage[i - 1].src.column)
		{
			// replace org column with new column

			yDstCol = Dcol;
			yDstPos = ColLen[Dcol - 1];

			// dst column is now bigger
			// note that this is a rare time when ColLen[] is changed before
			// MoveCard ()

			ColLen[Dcol - 1]++;
		}
		else
		{
			// move back to freecell for storage again

			yDstCol = gStorage[i - 1].src.column;
			yDstPos = gStorage[i - 1].src.position;
		}

		Scard = card[yStgCol][yStgPos];
		MoveCard (yStgCol, yStgPos, EMPTY, FALSE, TRUE);
		MoveCard (yDstCol, yDstPos, Scard, FALSE, TRUE);
	}
}
#ifdef SSMOVE

void MakeMoveHSS(CHAR src, CHAR dest) 
	{
		if ((src == Osrc) && MultMoveXS)
		  {
		  if (sccnt * (FvacO + 1) >= MultMoveXS)
			  return ;
		  sccnt++ ;
		  }
if (HSSignal == 0)
	HSSignal = 1 ;
else
	HSSignal = 2 ;
		if (!MakeMoveH(src, dest))
		{
			return ;
		}
		gbMoveID = !gbMoveID ;
		SSMScript[smcnt] = src ;
		SSMScript[smcnt + 1] = dest ;
		smcnt += 2 ;
		return ;
	}
void MakeMoveSS1(CHAR src, CHAR dest) 
	{
		MakeMoveHSS(src, LOBYTE(ColUsed[0] + '0')) ;
		MakeMoveHSS(src, LOBYTE(ColUsed[1] + '0')) ;
		MakeMoveHSS(LOBYTE(ColUsed[0] + '0'), LOBYTE(ColUsed[1] + '0')) ;
		MakeMoveHSS(src, LOBYTE(ColUsed[0] + '0')) ;
		return ;
	}
void MakeMoveSS2(CHAR src, CHAR dest) 
	{
		MakeMoveSS1(src, dest) ;
		MakeMoveHSS(src, dest) ;
		MakeMoveHSS(LOBYTE(ColUsed[0] + '0'), dest) ;
		MakeMoveHSS(LOBYTE(ColUsed[1] + '0'), dest) ;
		return ;
	}

void MakeMoveSS2A(CHAR src, CHAR dest)  // Special treatment for 6-card move with 0 empty freecells and  
	{									// four empty columns -- doesn't fit the general scheme
		MakeMoveHSS(src, LOBYTE(ColUsed[0] + '0')) ;
		MakeMoveHSS(src, dest) ;
		MakeMoveHSS(LOBYTE(ColUsed[0] + '0'), dest) ;
		return ;
	}

void MakeMoveSS3(CHAR src, CHAR dest) 
	{
		MakeMoveSS1(src, dest) ;
		MakeMoveHSS(src, LOBYTE(ColUsed[2] + '0')) ;
		MakeMoveHSS(LOBYTE(ColUsed[0] + '0'), LOBYTE(ColUsed[2] + '0')) ;
		MakeMoveHSS(LOBYTE(ColUsed[1] + '0'), LOBYTE(ColUsed[2] + '0')) ;
		if (!Spec46)
			MakeMoveSS2(src, dest) ;
		else
			MakeMoveSS2A(src, dest) ;  // Special for Cvac=4, MultMove=6
		MakeMoveHSS(LOBYTE(ColUsed[2] + '0'), LOBYTE(ColUsed[0] + '0')) ;
		MakeMoveHSS(LOBYTE(ColUsed[2] + '0'), LOBYTE(ColUsed[1] + '0')) ;
		MakeMoveHSS(LOBYTE(ColUsed[0] + '0'), LOBYTE(ColUsed[1] + '0')) ;
		MakeMoveHSS(LOBYTE(ColUsed[2] + '0'), dest) ;
		MakeMoveHSS(LOBYTE(ColUsed[1] + '0'), dest) ;
		return ;
	}
#endif

BOOL MakeMoveHS(CHAR src, CHAR dest)  
  {
  BOOL rtn ;
  CHAR col, coln ;
  INT i, mmcnt ;
#ifdef SSMOVE
  CARD CdestO ;
  INT CvacO ;
#endif
  //Used by MakeMoveStatus to determine if move is OK
  rtn = MakeMoveT(src, dest) ;
  FcMode = 1 ;
  FvacO = Fvac ;  // save original empty freecells 
  smcnt = 0 ;
  if (!rtn || !MultMove)
    {
    if (!MultMove)      
		MakeMoveH(src, dest) ;
    return rtn ;
    }
  else
    {
	for (i = 1 ; i < MAXCOL ; i++)
      ColUsed[i - 1] = 0 ;
#ifdef SSMOVE
	if (!MultMoveX)
	{
#endif
	MultMoveS = MultMove ;
    mmcnt = 0 ;
	MultMoveXS = 0 ;
// first, we climb up through the empty columns which can be
// used for supermove
  for (i = 0 ; i < MultMove / (FvacO + 1) ; i++)
      {
      for (col = 1 ; col < MAXCOL ; col++)
		{
	if (((card[col][0] == EMPTY) && (col != (dest - '0'))) &&
		(mmcnt < 100*MultMoveS / (100*(FvacO + 1) + 1))) //8/5/99 right? 
	{
	  coln = col + '0' ;
	  MakeMoveHSS(src, coln) ;
	  ColUsed[col - 1] = 1 ;
	  mmcnt++ ;
	  }
	}
	  MakeMoveHSS(src, dest) ;      // This is the middle move
      }
// then we work our way back down, disbursing the stored columns
    for (i = mmcnt ; i > 0 ; i--)
      {
      for (col = MAXCOL - 1 ; col > 0 ; col--)
	{
	if (ColUsed[col - 1])
	  {
	  coln = col + '0' ;
	  MakeMoveHSS(coln, dest) ;
	  }	  
	}
      }
#ifdef SSMOVE
  }
else   //Supersupermove
	{
if ((gbSolver2 || PlayMode2))
{
FcMode = 2;
}
	HSSignal = 0 ;
	if ((MultMoveX == 6) && (Cvac == 4))
		Spec46 = TRUE ;
	MultMoveXS = MultMoveX ;
	i = 0 ;
    for (col = 1 ; col < MAXCOL ; col++)  //list positions of empty columns
	  {
	  if ((card[col][0] == EMPTY) && (col != (dest - '0')))
	    {
		ColUsed[i] = col ;
		i++ ;
	    }
	  }
	  CdestO = card[dest - '0'][0] ; // save top card of destination column
	  CvacO	= Cvac ; // save original Cvac
	  Osrc = src ;
	  sccnt = 0 ;
	  if (((CvacO == 3) && (CdestO == EMPTY)) ||
			((CvacO == 2) && (CdestO != EMPTY)))
	  {
		MakeMoveSS2(src, dest) ;
	  }
	if (((CvacO == 4) && (CdestO == EMPTY)) ||
			((CvacO == 3) && (CdestO != EMPTY)))
	    {
		if (!SMLim)
		{
			MakeMoveSS3(src, dest) ;
		}
		else
			MakeMoveSS2(src, dest) ;
	    }
	  if (((CvacO >= 5) && (CdestO == EMPTY)) ||
			((CvacO >= 4) && (CdestO != EMPTY)))
	    {
		if (!SMLim)
		  {
		  MakeMoveSS1(src, dest) ;
		  MakeMoveHSS(src, LOBYTE(ColUsed[3] + '0')) ;
		  MakeMoveHSS(LOBYTE(ColUsed[0] + '0'), LOBYTE(ColUsed[3] + '0')) ;
		  MakeMoveHSS(LOBYTE(ColUsed[1] + '0'), LOBYTE(ColUsed[3] + '0')) ;
		  MakeMoveSS3(src, dest) ;
		  MakeMoveHSS(LOBYTE(ColUsed[3] + '0'), dest) ;
		  }
		else
		  {
		  if (SMLim == 1)
		  {
		    MakeMoveSS3(src, dest) ;
		  }
		  else
		    MakeMoveSS2(src, dest) ;
		  }
		}
	  }
//	FcMode = 0 ;
//	SupMove = 1 ;
#endif  // end of supersupermove
    gbMoveID = !gbMoveID ;  // reverse final toggle
	Spec46 = FALSE ;
  }
  return TRUE ;
  }

//
//Called from GotLiveClick to check if supermove had been used,
//and stores source character for one move at a time if so, returning
//destination character for saving by GotLiveClick
//Return signals when finished by zero in upper byte
//
WORD SaveMultMove(CHAR loc, CHAR Scol)  
{
	if (!MultMoveS && !MultMoveXS)
		return (WORD)loc ;
	else
	{
		if (loc)  // first pass
		{
			dcolc = loc - '0' ;	
			mcurcol = 1 ;
			mupind = mmcnt = 0 ;
		}
		if (mmcnt)
			{
			gSel.location = 1 ;
			LogMove(SSMScript[mmcnt]) ;
			gSel.location = 0 ;
			}
		mmcnt += 2 ;
		if (mmcnt < smcnt)
			return (256 + SSMScript[mmcnt - 1]) ; 
		else
			{
			MultMoveXS = MultMoveS = 0 ;
			return ((WORD)dcolc + '0') ;  // signal last move
			}
	}
	return 0 ;  // safety, should never get here
}

VOID TestPossMoves(char loc)
{
	CHAR i, tcol;
	BOOL poss_moves[FCPLAYsizePROAID_POSS_MOVES];

	if (!loc)
	{
		// move == 0 means clear

		for (i = 0 ; i < FCPLAYsizePROAID_POSS_MOVES; i++)
		{
			// make copy
			poss_moves[i] = PossMoves[i];
			// clear
			PossMoves[i] = FALSE;
		}
	}
	else
	{
		// check locations, 8 columns and 4 hcs

		for (i = 0 ; i < FCPLAYsizePROAID_POSS_MOVES; i++)
		{
			// make copy

			poss_moves[i] = PossMoves[i];

			// calc new

			if (i < NUM_COLS)
			{
				// columns
				tcol = i + '1';
			}
			else 
			{
				// 4 hcs: e, f, g & h
				tcol = i - 4 + 'a';
			}

			if (tcol != loc) 
			{
				PossMoves[i] = MakeMoveTP(loc, tcol) ;
			}
			else
			{
				// cannot move onto yourself
				PossMoves[i] = FALSE ;
			}
		}
	}

	// check copy for invalidations

	for (i = 0; i <	FCPLAYsizePROAID_POSS_MOVES; i++)
	{
		if (poss_moves[i] != PossMoves[i])
		{
			InvalidateArrow (i);
		}
	}

	return ;
}

VOID TestProLines (VOID)
{
	INT posS_tmp, zCol, col, st;
	BYTE lineState[NUM_COLS][FCPLAYstatesPROLINE];
	for (col = 1, zCol = 0; col < MAXCOL; col++, zCol++)
	{
		// prepare proaid for this column

		for (st = 0; st < FCPLAYstatesPROLINE; st++)
		{
			// make copy
			lineState[zCol][st] = posS[zCol][st];
			// init
			posS[zCol][st] = MAXPOS;
		}

		if (ColLen[zCol] > 1)
		{
			// 2 or more cards in the column - calc line start position

			for (posS_tmp = ColLen[zCol] - 2 ; posS_tmp >= 0 ; posS_tmp--)
			{
				if (((VALUE(card[col][posS_tmp]) - 1) 
									== VALUE(card[col][posS_tmp + 1])) &&
					((COLOUR(card[col][posS_tmp]) 
									!= COLOUR(card[col][posS_tmp + 1]))))
				{
					posS[zCol][0] = posS_tmp;
				}
				else
				{
					break;  
				}
			}
		}	

		if (posS[zCol][0] != MAXPOS)
		{
			// there is a line - calculate hashes

			if ((!Cvac) && (VALUE(card[col][posS[zCol][0]]) != KING))
			{
				// there is no empty column and line doesnt start on king
				// - calc double hash

				if ((ColLen[zCol] - posS[zCol][0] - 1) <= Fvac)
					posS[zCol][2] = posS[zCol][0] ;
				else
					posS[zCol][2] = ColLen[zCol] - (Fvac + 1) * (Cvac + 1) ;
			}
			if (Cvac)
			{
				// there is an empty column - calc hashes

				if ((ColLen[zCol] - posS[zCol][0] - 1)
#ifndef SSMOVEP
					< (1 + SupMove * (Cvac - 1)) * (Fvac + 1))
#else
					< (1 + SupMove * (1<<(Cvac - 2)) * (Fvac + 1)))
#endif
				{
					// calc triple or single hash

					if (VALUE(card[col][posS[zCol][0]]) != KING)
						posS[zCol][3] = posS[zCol][0];
					else
						if (posS[zCol][0])
						  posS[zCol][1] = posS[zCol][0];
				}
				else
				{
					// calc single & double hash
					if (!SupMove)
						posS[zCol][1] = ColLen[zCol] - (Fvac + 1) ;
					else
#ifndef SSMOVEP
						posS[zCol][1] = ColLen[zCol] - (Fvac + 1) * Cvac ;
#else
						posS[zCol][1] = ColLen[zCol] - (1<<(Cvac - 1)) * (Fvac + 1) ;
#endif
					if ((ColLen[zCol] - posS[zCol][0] - 1) < 
#ifndef SSMOVEP
							(Fvac + 1) * (Cvac + 1))
#else
							(Fvac + 1) * (Cvac + 1) + 
							SupMove * ((1<<(Cvac)) * (Fvac + 1) - (Fvac + 1) * (Cvac + 1)))
#endif
						posS[zCol][2] = posS[zCol][0] ;
					else
#ifndef SSMOVEP
						posS[zCol][2] = ColLen[zCol] - (Fvac + 1) * (Cvac + 1) ;
#else
						posS[zCol][2] = ColLen[zCol] - ((Fvac + 1) * (Cvac + 1) +
							SupMove * ((1<<(Cvac)) * (Fvac + 1) - (Fvac + 1) * (Cvac + 1))) ;
#endif
  if (VALUE(card[col][posS[zCol][2]]) == KING)
						posS[zCol][2] = ColLen[zCol] ; // cancel if king
				}
			}
		} 
	}

	// check copy for invalidations

	for (zCol = 0; zCol < NUM_COLS; zCol++)
	{
		if (lineState[zCol][0] != posS[zCol][0])
		{
			InvalidateProLine (zCol, lineState[zCol][0]); // old column
			InvalidateProLine (zCol, posS[zCol][0]); // new column
		}

		if (lineState[zCol][3] != posS[zCol][3])
		{
			InvalidateProHash3 (zCol, lineState[zCol][3]);
			InvalidateProHash3 (zCol, posS[zCol][3]);
		}
		if (lineState[zCol][2] != posS[zCol][2])
		{
			InvalidateProHash2 (zCol, lineState[zCol][2]);
			InvalidateProHash2 (zCol, posS[zCol][2]);
		}
		if (lineState[zCol][1] != posS[zCol][1])
		{
			InvalidateProHash1 (zCol, lineState[zCol][1]);
			InvalidateProHash1 (zCol, posS[zCol][1]);
		}
	}
	return ;
}
