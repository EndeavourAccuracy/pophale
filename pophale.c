/* SPDX-License-Identifier: GPL-3.0-or-later */
/* pophale v1.0 (December 2022)
 * Copyright (C) 2018-2022 Norbert de Jonge <nlmdejonge@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see [ www.gnu.org/licenses/ ].
 *
 * To properly read this code, set your program's tab stop to: 2.
 */

/*========== Includes ==========*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#if defined WIN32 || _WIN32 || WIN64 || _WIN64
#include <windows.h>
#undef PlaySound
#endif

#include <zip.h>

#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_thread.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
/*========== Includes ==========*/

/*========== Defines ==========*/
#if defined WIN32 || _WIN32 || WIN64 || _WIN64
#define SLASH "\\"
#define DEVNULL "NUL"
#else
#define SLASH "/"
#define DEVNULL "/dev/null"
#endif

#define EXIT_NORMAL 0
#define EXIT_ERROR 1
#define EDITOR_NAME "pophale"
#define EDITOR_VERSION "v1.0 (December 2022)"
#define COPYRIGHT "Copyright (C) 2022 Norbert de Jonge"
#define WINDOW_WIDTH 712
#define WINDOW_HEIGHT 682
#define DIR_JAR "jar"
#define DIR_UNCOMP "uncomp"
#define BACKUP DIR_JAR SLASH "backup.bak"
#define MAX_PATHFILE 200
#define MAX_ERROR 200
#define MAX_WARNING 200
#define MAX_OPTION 100
#define MAX_LEVEL 6
#define MAX_DATA 1000
#define MAX_WIDTH 1000
#define MAX_HEIGHT 1000
#define MAX_UNKNOWN 100
#define MAX_FRONT_TYPE 100
#define MAX_FRONT 1000
#define MAX_CHOMPERS 1000
#define MAX_SPIKES 1000
#define MAX_GATES 1000
#define MAX_RAISE 1000
#define MAX_GUARDS 1000
#define MAX_POTIONS 1000
#define MAX_LOOSE 1000
#define MAX_TEXT 65000
#define MAX_IMG 200
#define MAX_CON 30
#define NUM_SOUNDS 20 /*** Sounds that may play at the same time. ***/
#define BAR_FULL 658
#define REFRESH_PROG 25 /*** That is 40 fps (1000/25). ***/
#define REFRESH_GAME 33 /*** That is ~30 fps (1000/33). ***/
#define FPS_TORCH 33
#define MAX_FILE 100
#define MAX_ZOOM 7
#define DEFAULT_ZOOM 4
#define MAP_WIDTH 704
#define MAP_HEIGHT 624
#define MAP_LEFT 4
#define MAP_TOP 29
#define MAX_TOWRITE 720
#define MAX_LINECHARS 24
#define MAX_LINES 12
#define MAX_STATUS 100
#define MAX_FONT_SIZE 70

#define PNG_VARIOUS "various"
#define PNG_BACK "back"
#define PNG_BUTTONS "buttons"
#define PNG_GAMEPAD "gamepad"
#define PNG_FRONT "front"

#ifndef O_BINARY
#define O_BINARY 0
#endif
/*========== Defines ==========*/

/*** This is a single level. ***/
unsigned long ulWidth, ulHeight;
unsigned long ulBack[MAX_HEIGHT + 2][MAX_WIDTH + 2];
unsigned long ulNrUnknown;
unsigned long ulUnknownA[MAX_UNKNOWN + 2];
unsigned long ulUnknownB[MAX_UNKNOWN + 2];
unsigned long ulUnknownC[MAX_UNKNOWN + 2];
unsigned long ulUnknownD[MAX_UNKNOWN + 2];
unsigned long ulFrontTypes;
unsigned long ulFrontTypeA[MAX_FRONT_TYPE + 2];
unsigned long ulFrontTypeB[MAX_FRONT_TYPE + 2];
unsigned long ulFrontTypeNr[MAX_FRONT_TYPE + 2];
unsigned long ulNrFront;
unsigned long ulFrontX[MAX_FRONT + 2];
unsigned long ulFrontY[MAX_FRONT + 2];
unsigned long ulFrontType[MAX_FRONT + 2];
unsigned long ulFrontA[MAX_FRONT + 2];
unsigned long ulFrontB[MAX_FRONT + 2];
unsigned long ulPrinceX, ulPrinceY;
unsigned long ulExitTriggerX, ulExitTriggerY;
unsigned long ulSaveTriggerX, ulSaveTriggerY;
unsigned long ulEntranceImageX, ulEntranceImageY;
unsigned long ulExitImageX, ulExitImageY;
unsigned long ulNrChompers;
unsigned long ulChomperX[MAX_CHOMPERS + 2];
unsigned long ulChomperY[MAX_CHOMPERS + 2];
unsigned long ulChomperA[MAX_CHOMPERS + 2];
unsigned long ulNrSpikes;
unsigned long ulSpikeX[MAX_SPIKES + 2];
unsigned long ulSpikeY[MAX_SPIKES + 2];
unsigned long ulSpikeRight[MAX_SPIKES + 2];
unsigned long ulNrGates;
unsigned long ulGateX[MAX_GATES + 2];
unsigned long ulGateY[MAX_GATES + 2];
unsigned long ulGateTimeOpen[MAX_GATES + 2];
unsigned long ulNrRaise;
unsigned long ulRaiseGate[MAX_RAISE + 2];
unsigned long ulRaiseX[MAX_RAISE + 2];
unsigned long ulRaiseY[MAX_RAISE + 2];
unsigned long ulNrGuards;
unsigned long ulGuardX[MAX_GUARDS + 2];
unsigned long ulGuardY[MAX_GUARDS + 2];
unsigned long ulGuardDir[MAX_GUARDS + 2];
unsigned long ulGuardHP[MAX_GUARDS + 2];
unsigned long ulGuardA[MAX_GUARDS + 2];
unsigned long ulGuardB[MAX_GUARDS + 2];
unsigned long ulGuardC[MAX_GUARDS + 2];
unsigned long ulGuardD[MAX_GUARDS + 2];
unsigned long ulGuardE[MAX_GUARDS + 2];
unsigned long ulGuardF[MAX_GUARDS + 2];
unsigned long ulNrPotions;
unsigned long ulPotionType[MAX_POTIONS + 2];
unsigned long ulPotionX[MAX_POTIONS + 2];
unsigned long ulPotionY[MAX_POTIONS + 2];
unsigned long ulNrLoose;
unsigned long ulLooseX[MAX_LOOSE + 2];
unsigned long ulLooseY[MAX_LOOSE + 2];
unsigned long ulLooseRight[MAX_LOOSE + 2];
unsigned long ulNrText;
unsigned char sText[MAX_TEXT + 2];

int iNrLines;
char arTextLine[MAX_LINES + 2][MAX_LINECHARS + 2];

int iDebug;
int iJAR;
char sPathFile[MAX_PATHFILE + 2];
int iStartLevel;
char sError[MAX_ERROR + 2];
char sWarning[MAX_WARNING + 2];
SDL_Window *window;
SDL_Renderer *ascreen;
int iScale;
int iFullscreen;
SDL_Cursor *curArrow;
SDL_Cursor *curWait;
SDL_Cursor *curHand;
SDL_Cursor *curText;
int iNoAudio;
int iNoController;
TTF_Font *font[MAX_FONT_SIZE + 2];
int iPreLoaded;
int iCurrentBarHeight;
int iNrToPreLoad;
unsigned int gamespeed;
Uint32 looptime;
int iCurLevel;
int iDownAt;
int iZoom;
int iXPos, iYPos;
int iDraggingMap;
int iXPosDragStart, iYPosDragStart;
int iDraggingMoved;
int iXPosDragOffset, iYPosDragOffset;
int iNoAnim;
int iFlameFrame;
Uint32 newticks, oldticks;
Uint32 oldticksf;
char arText[9 + 2][MAX_TEXT + 2];
int iTextOK;
int iHelpOK;
int iHoverRow, iHoverColumn;
int iCloseOn;
int iYesOn;
int iNoOn;
int iEXESave;
int iOnTile;
int iChanged;
struct zip *zip;
int iEmulator;
int iLastBack;
int iDelWhat, iDelX, iDelY;
int iDelXPos, iDelYPos;
int iLine;
int iXYNear;
int iChangeBack;
int iChangeFront;
int iCFX, iCFY;
int iOKOn;
int iHideFront;

/*** front ***/
int iFront0101;
int iFront0102;
int iFront0201;
int iFront0202;
int iFront0301;
int iFront0302;
int iFront0401;
int iFront0402;
int iFront0501;
int iFront0502;
int iFront0601;
int iFront0602;
int iFront0701;
int iFront0702;
int iFront1301;
int iFront1401;
int iFront1501;
int iFront1601;
int iFront1701;
int iFront1702;
int iFront1703;
int iFront1704;
int iFront1705;
int iFront1706;
int iFront1707;
int iFront1708;
int iFront1801;
int iFront1901;

/*** EXE ***/
int iEXEMenuInitialSelection;
int iEXECutscenesFontEmphasis;
int iEXECutscenesTextLines;
int iStatusBarFrame;
char sStatus[MAX_STATUS + 2], sStatusOld[MAX_STATUS + 2];

/*** for text ***/
SDL_Color color_bl = {0x00, 0x00, 0x00, 255};
SDL_Color color_wh = {0xff, 0xff, 0xff, 255};
SDL_Color color_red = {0xff, 0x00, 0x00, 255};
SDL_Color color_blue = {0x00, 0x00, 0xff, 255};
SDL_Color color_f4 = {0xf4, 0xf4, 0xf4, 255};
SDL_Surface *message;
SDL_Texture *messaget;
SDL_Rect offset;

/*** controller ***/
int iController;
SDL_GameController *controller;
char sControllerName[MAX_CON + 2];
SDL_Joystick *joystick;
SDL_Haptic *haptic;
Uint32 joyleft;
Uint32 joyright;
Uint32 joyup;
Uint32 joydown;
Uint32 trigleft;
Uint32 trigright;

SDL_Texture *imgloading;
SDL_Texture *imgblack;
SDL_Texture *imginterface;
SDL_Texture *imgexe;
SDL_Texture *imgtext;
SDL_Texture *imghovertext;
SDL_Texture *imghelp;
SDL_Texture *imghoverbacks;
SDL_Texture *imghoverbackl;
SDL_Texture *imghoverbackllive;
SDL_Texture *imghoverfront;
SDL_Texture *imgback[55 + 2];
SDL_Texture *imgbacktiles;
SDL_Texture *imgfront;
SDL_Texture *imgprevoff, *imgprevon_0, *imgprevon_1;
SDL_Texture *imgnextoff, *imgnexton_0, *imgnexton_1;
SDL_Texture *imgexeoff, *imgexeon_0, *imgexeon_1;
SDL_Texture *imgtextoff, *imgtexton_0, *imgtexton_1;
SDL_Texture *imgplayoff, *imgplayon_0, *imgplayon_1;
SDL_Texture *imghelpoff, *imghelpon_0, *imghelpon_1;
SDL_Texture *imgsaveoff, *imgsaveon_0, *imgsaveon_1;
SDL_Texture *imgsave[2 + 2];
SDL_Texture *imgok[2 + 2];
SDL_Texture *imgclose[2 + 2];
SDL_Texture *imgentrance;
SDL_Texture *imgexit;
SDL_Texture *imgprincer;
SDL_Texture *imgexittrigger;
SDL_Texture *imgsavetrigger;
SDL_Texture *imgchomper;
SDL_Texture *imgspikes0;
SDL_Texture *imgspikes1;
SDL_Texture *imggate;
SDL_Texture *imgraise;
SDL_Texture *imgguardr;
SDL_Texture *imgguardl;
SDL_Texture *imgpotion0;
SDL_Texture *imgpotion1;
SDL_Texture *imgpotion2;
SDL_Texture *imgpotion3;
SDL_Texture *imgloose0;
SDL_Texture *imgloose1;
SDL_Texture *imgtorchsprite;
SDL_Texture *imgpillarfront;
SDL_Texture *imgskeleton;
SDL_Texture *imgwalltopleftslash;
SDL_Texture *imgwalltopleftdot;
SDL_Texture *imgfloorclimbable;
SDL_Texture *imgwallbottomleft;
SDL_Texture *imgfaded;
SDL_Texture *imgpopup_yn;
SDL_Texture *imgyes[2 + 2];
SDL_Texture *imgno[2 + 2];
SDL_Texture *imgquit_0;
SDL_Texture *imgquit_1;
SDL_Texture *imgemulator;
SDL_Texture *imgdelete;
SDL_Texture *imgstatusbarsprite;
SDL_Texture *imgpopup;
SDL_Texture *imgchkb;

struct sample {
	Uint8 *data;
	Uint32 dpos;
	Uint32 dlen;
} sounds[NUM_SOUNDS];

static const char *sMeta[9] =
	{ "Manifest-Version: 1.0", "MicroEdition-Configuration: CLDC-1.0", "MicroEdition-Profile: MIDP-2.0", "MIDlet-Name: Prince of Persia: Harem Adventures", "MIDlet-1: Prince of Persia: Harem Adventures, popicon.png, PrinceOfPersia", "MIDlet-Icon: popicon.png", "MIDlet-Version: 1.0.9", "MIDlet-Vendor: Gameloft", "Nokia-MIDlet-Category: Game" };

void ShowUsage (void);
void PrIfDe (char *sString);
void CreateDir (char *sDir);
void GetOptionValue (char *sArgv, char *sValue);
void ErrorAndExit (void);
void Warning (void);
void SetPathFile (void);
void Decompress (void);
unsigned long ReadFromFile (int iFd, char *sWhat,
	int iSize, unsigned char *sRetString);
unsigned long BytesAsLU (unsigned char *sData, int iBytes);
void LevelLoad (int iLevel);
void InitScreenAction (char *sAction);
void InitScreen (void);
void ShowFrontTiles (void);
void ShowScreen (void);
void Quit (void);
void LoadFonts (void);
void MixAudio (void *unused, Uint8 *stream, int iLen);
void PlaySound (char *sFile);
void PreLoad (char *sPath, char *sPNG, SDL_Texture **imgImage);
void ShowImage (SDL_Texture *img, int iX, int iY, char *sImageInfo,
	SDL_Renderer *screen, float fMultiply, int iXYScale);
void LoadingBar (int iBarHeight);
void PrevLevel (void);
void NextLevel (void);
void SetZoom (int iZoomTo);
int InArea (int iUpperLeftX, int iUpperLeftY,
	int iLowerRightX, int iLowerRightY);
int MapStartX (void);
int MapStartY (void);
void DisplayText (int iStartX, int iStartY, int iFontSize,
	int iLines, SDL_Color back, int iXYScale);
void CustomRenderCopy (SDL_Texture* src, SDL_Rect* srcrect,
	SDL_Rect *dstrect, char *sImageInfo, int iXYScale);
void Help (void);
void ShowHelp (void);
void OpenURL (char *sURL);
void ChangeBackAction (char *sAction);
void ChangeBack (void);
void ShowChangeBack (void);
int TileHexToNr (int iHex);
int TileNrToHex (int iNr);
void ChangeFrontAction (char *sAction);
void ChangeFront (void);
void ShowChangeFront (void);
void CenterNumber (int iNumber, int iX, int iY,
	SDL_Color fore, int iHex);
int PlusMinus (int *iWhat, int iX, int iY,
	int iMin, int iMax, int iChange, int iAddChanged);
void LevelSave (void);
void CreateBAK (void);
void WriteByte (int iFd, int iValue);
void WriteWord (int iFd, int iValue);
void PopUpSave (void);
void ShowPopUpSave (void);
void Zoom (int iToggleFull);
void Compress (void);
int ZIPDir (char *sDir);
int ZIPFile (char *sFrom, char *sTo);
void Playtest (void);
int StartGame (void *unused);
void AddFront (int iX, int iY, int iFrontTypeNr);
void ClearLevel (void);
void Text (void);
void ShowText (void);
void EXE (void);
void ShowEXE (void);
int OpenClass (char *sClass, char cReadOrWrite);
void EXELoad (void);
void EXESave (void);
void UpdateStatusBar (void);
int AlignX (int iX, int iOverSixteen);
int AlignY (int iY, int iOverTwentyFour);
void DelWhat (int iType, int iX, int iY);
void Del (void);
void CreateMeta (char *sFile);
void VerifyVersion (void);
void InitPopUp (void);
void ShowPopUp (void);
void UpdateHover (void);

/*****************************************************************************/
int main (int argc, char *argv[])
/*****************************************************************************/
{
	char sStartLevel[MAX_OPTION];

	/*** Used for looping. ***/
	int iArgLoop;

	iDebug = 0;
	iStartLevel = 0;
	iScale = 1;
	iFullscreen = 0;
	iNoAudio = 0;
	iNoController = 0;
	iZoom = DEFAULT_ZOOM;
	iNoAnim = 0;
	iEmulator = 0;
	iLastBack = 0x04;
	iChanged = 0;
	iDelX = -1;
	iDelY = -1;
	iHideFront = 0;

	if (argc > 1)
	{
		for (iArgLoop = 1; iArgLoop <= argc - 1; iArgLoop++)
		{
			if ((strcmp (argv[iArgLoop], "-h") == 0) ||
				(strcmp (argv[iArgLoop], "-?") == 0) ||
				(strcmp (argv[iArgLoop], "--help") == 0))
			{
				ShowUsage();
			}
			else if ((strcmp (argv[iArgLoop], "-v") == 0) ||
				(strcmp (argv[iArgLoop], "--version") == 0))
			{
				printf ("%s %s\n", EDITOR_NAME, EDITOR_VERSION);
				exit (EXIT_NORMAL);
			}
			else if ((strcmp (argv[iArgLoop], "-d") == 0) ||
				(strcmp (argv[iArgLoop], "--debug") == 0))
			{
				iDebug = 1;
			}
			else if ((strcmp (argv[iArgLoop], "-n") == 0) ||
				(strcmp (argv[iArgLoop], "--noaudio") == 0))
			{
				iNoAudio = 1;
			}
			else if ((strcmp (argv[iArgLoop], "-z") == 0) ||
				(strcmp (argv[iArgLoop], "--zoom") == 0))
			{
				iScale = 2;
			}
			else if ((strcmp (argv[iArgLoop], "-f") == 0) ||
				(strcmp (argv[iArgLoop], "--fullscreen") == 0))
			{
				iFullscreen = SDL_WINDOW_FULLSCREEN_DESKTOP;
			}
			else if ((strncmp (argv[iArgLoop], "-l=", 3) == 0) ||
				(strncmp (argv[iArgLoop], "--level=", 8) == 0))
			{
				GetOptionValue (argv[iArgLoop], sStartLevel);
				iStartLevel = atoi (sStartLevel);
				if ((iStartLevel < 0) || (iStartLevel > MAX_LEVEL))
					{ iStartLevel = 0; }
			}
			else if ((strcmp (argv[iArgLoop], "-s") == 0) ||
				(strcmp (argv[iArgLoop], "--static") == 0))
			{
				iNoAnim = 1;
			}
			else if ((strcmp (argv[iArgLoop], "-k") == 0) ||
				(strcmp (argv[iArgLoop], "--keyboard") == 0))
			{
				iNoController = 1;
			}
			else
			{
				ShowUsage();
			}
		}
	}

	SetPathFile();
	Decompress();
	VerifyVersion();
	LevelLoad (iStartLevel);

	InitScreen();
	Quit();

	return 0;
}
/*****************************************************************************/
void ShowUsage (void)
/*****************************************************************************/
{
	printf ("%s %s\n%s\n\n", EDITOR_NAME, EDITOR_VERSION, COPYRIGHT);
	printf ("Usage:\n");
	printf ("  %s [OPTIONS]\n\nOptions:\n", EDITOR_NAME);
	printf ("  -h, -?,    --help           display this help and exit\n");
	printf ("  -v,        --version        output version information and"
		" exit\n");
	printf ("  -d,        --debug          also show levels on the console\n");
	printf ("  -n,        --noaudio        do not play sound effects\n");
	printf ("  -z,        --zoom           double the interface size\n");
	printf ("  -f,        --fullscreen     start in fullscreen mode\n");
	printf ("  -l=NR,     --level=NR       start in level NR\n");
	printf ("  -s,        --static         do not display animations\n");
	printf ("  -k,        --keyboard       do not use a game controller\n");
	printf ("\n");
	exit (EXIT_NORMAL);
}
/*****************************************************************************/
void PrIfDe (char *sString)
/*****************************************************************************/
{
	if (iDebug == 1) { printf ("%s", sString); }
}
/*****************************************************************************/
void CreateDir (char *sDir)
/*****************************************************************************/
{
	struct stat stStatus;

	if (stat (sDir, &stStatus) == -1)
	{
#if defined WIN32 || _WIN32 || WIN64 || _WIN64
mkdir (sDir);
#else
mkdir (sDir, 0700);
#endif
	}
}
/*****************************************************************************/
void GetOptionValue (char *sArgv, char *sValue)
/*****************************************************************************/
{
	int iTemp;
	char sTemp[MAX_OPTION + 2];

	iTemp = strlen (sArgv) - 1;
	snprintf (sValue, MAX_OPTION, "%s", "");
	while (sArgv[iTemp] != '=')
	{
		snprintf (sTemp, MAX_OPTION, "%c%s", sArgv[iTemp], sValue);
		snprintf (sValue, MAX_OPTION, "%s", sTemp);
		iTemp--;
	}
}
/*****************************************************************************/
void ErrorAndExit (void)
/*****************************************************************************/
{
	printf ("[FAILED] %s\n", sError);
	SDL_ShowSimpleMessageBox (SDL_MESSAGEBOX_ERROR,
		"Error", sError, NULL);
	exit (EXIT_ERROR);
}
/*****************************************************************************/
void Warning (void)
/*****************************************************************************/
{
	printf ("[ WARN ] %s\n", sWarning);
	SDL_ShowSimpleMessageBox (SDL_MESSAGEBOX_ERROR,
		"Warning", sWarning, NULL);
}
/*****************************************************************************/
void SetPathFile (void)
/*****************************************************************************/
{
	int iFound;
	DIR *dDir;
	struct dirent *stDirent;
	char sExtension[100 + 2];

	iFound = 0;
	dDir = opendir (DIR_JAR);
	if (dDir == NULL)
	{
		snprintf (sError, MAX_ERROR, "%s: %s!", DIR_JAR, strerror (errno));
		iJAR = 0;
	} else {
		while ((stDirent = readdir (dDir)) != NULL)
		{
			if (iFound == 0)
			{
				if ((strcmp (stDirent->d_name, ".") != 0) &&
					(strcmp (stDirent->d_name, "..") != 0))
				{
					snprintf (sExtension, 100, "%s", strrchr (stDirent->d_name, '.'));
					if ((toupper (sExtension[1]) == 'J') &&
						(toupper (sExtension[2]) == 'A') &&
						(toupper (sExtension[3]) == 'R'))
					{
						iFound = 1;
						snprintf (sPathFile, MAX_PATHFILE, "%s%s%s", DIR_JAR, SLASH,
							stDirent->d_name);
						if (iDebug == 1)
						{
							printf ("[ INFO ] Found JAR file \"%s\".\n", stDirent->d_name);
						}
					}
				}
			}
		}
		if (iFound == 0)
		{
			snprintf (sError, MAX_ERROR, "Could not find a JAR file in %s%s!",
				DIR_JAR, SLASH);
			iJAR = 0;
		} else {
			if (access (sPathFile, R_OK|W_OK) == -1)
			{
				snprintf (sError, MAX_ERROR, "%s: %s!", sPathFile, strerror (errno));
				iJAR = 0;
			} else { iJAR = 1; }
		}
	}
	closedir (dDir);

	if (iJAR == 0) { ErrorAndExit(); }
}
/*****************************************************************************/
void Decompress (void)
/*****************************************************************************/
{
	struct zip_stat zips;
	struct zip_file *zipf;
	/***/
	char sZipError[MAX_ERROR + 2];
	int iError;
	int iNrFiles;
	char sLocation[MAX_PATHFILE + 2];
	int iLength;
	int iFd;
	unsigned long ulTotal;
	int iChars;
	char sBuffer[100];

	/*** Used for looping. ***/
	int iFileLoop;

	CreateDir (DIR_UNCOMP);

	zip = zip_open (sPathFile, 0, &iError);
	if (zip == NULL)
	{
		zip_error_to_str (sZipError, sizeof (sZipError), iError, errno);
		snprintf (sError, MAX_ERROR, "Cannot open \"%s\": %s!",
			sPathFile, sZipError);
		ErrorAndExit();
	} else {
		iNrFiles = zip_get_num_entries (zip, 0);
		for (iFileLoop = 0; iFileLoop < iNrFiles; iFileLoop++)
		{
			if (zip_stat_index (zip, iFileLoop, 0, &zips) == 0)
			{
				snprintf (sLocation, MAX_PATHFILE, "%s%s%s",
					DIR_UNCOMP, SLASH, zips.name);
				iLength = strlen (zips.name);
				/* Note that checking for '/' may fail. It is also "not specified
				 * explicitly in the official format description".
				 * https://libzip.org/libzip-discuss/msg00257.html
				 */
				if (zips.name[iLength - 1] == '/')
				{
					CreateDir (sLocation);
				} else {
					zipf = zip_fopen_index (zip, iFileLoop, 0);
					iFd = open (sLocation, O_WRONLY|O_TRUNC|O_CREAT|O_BINARY, 0600);
					ulTotal = 0;
					while (ulTotal != zips.size)
					{
						iChars = zip_fread (zipf, sBuffer, 100);
						write (iFd, sBuffer, iChars);
						ulTotal+=iChars;
					}
					close (iFd);
					zip_fclose (zipf);
				}
			}
		}
		zip_close (zip);
		PrIfDe ("[  OK  ] Decompressed JAR file.\n");
	}
}
/*****************************************************************************/
unsigned long ReadFromFile (int iFd, char *sWhat,
	int iSize, unsigned char *sRetString)
/*****************************************************************************/
{
	int iLength;
	int iRead;
	char sRead[1 + 2];
	int iEOF;

	if ((iDebug == 1) && (strcmp (sWhat, "") != 0))
	{
		printf ("[  OK  ] Loading: %s\n", sWhat);
	}
	iLength = 0;
	iEOF = 0;
	do {
		iRead = read (iFd, sRead, 1);
		switch (iRead)
		{
		case -1:
				printf ("[FAILED] Could not read (1): %s!\n", strerror (errno));
				exit (EXIT_ERROR);
				break;
			case 0: PrIfDe ("[ INFO ] End of file\n"); iEOF = 1; break;
			default:
				sRetString[iLength] = sRead[0];
				iLength++;
				break;
		}
	} while ((iLength < iSize) && (iEOF == 0));
	sRetString[iLength] = '\0';

	if (strlen ((char *)sRetString) <= 4)
		{ return (BytesAsLU (sRetString, iSize)); }
			else { return (-1); }
}
/*****************************************************************************/
unsigned long BytesAsLU (unsigned char *sData, int iBytes)
/*****************************************************************************/
{
	unsigned long luReturn;
	char sString[MAX_DATA + 2];
	char sTemp[MAX_DATA + 2];
	int iTemp;

	snprintf (sString, MAX_DATA, "%s", "");
	for (iTemp = iBytes - 1; iTemp >= 0; iTemp--)
	{
		snprintf (sTemp, MAX_DATA, "%s%02x", sString, sData[iTemp]);
		snprintf (sString, MAX_DATA, "%s", sTemp);
	}
	luReturn = strtoul (sString, NULL, 16);

	return (luReturn);
}
/*****************************************************************************/
void LevelLoad (int iLevel)
/*****************************************************************************/
{
	char sLocation[MAX_PATHFILE + 2];
	int iFd;
	char sString[MAX_DATA + 2];
	unsigned char sData[MAX_DATA + 2];
	char sTempLine[MAX_LINECHARS + 2];

	/*** Used for looping. ***/
	int iRowLoop, iColumnLoop;
	int iLoop;

	snprintf (sLocation, MAX_PATHFILE, "%s%s%i.lvl", DIR_UNCOMP, SLASH, iLevel);
	iFd = open (sLocation, O_RDONLY|O_BINARY);
	if (iFd == -1)
	{
		snprintf (sError, MAX_ERROR, "Could not open \"%s\": %s!",
			sLocation, strerror (errno));
		ErrorAndExit();
	}

	/*** width and height ***/
	ulWidth = ReadFromFile (iFd, "", 1, sData);
	ulHeight = ReadFromFile (iFd, "", 1, sData);

	/*** back ***/
	for (iRowLoop = 1; iRowLoop <= (int)ulHeight; iRowLoop++)
	{
		ReadFromFile (iFd, "", ulWidth, sData);
		for (iColumnLoop = 1; iColumnLoop <= (int)ulWidth; iColumnLoop++)
		{
			snprintf (sString, MAX_DATA, "%02x", sData[iColumnLoop - 1]);
			ulBack[iRowLoop][iColumnLoop] = strtoul (sString, NULL, 16);
		}
	}

	/*** unknown ***/
	ulNrUnknown = ReadFromFile (iFd, "", 2, sData);
	for (iLoop = 1; iLoop <= (int)ulNrUnknown; iLoop++)
	{
		ulUnknownA[iLoop] = ReadFromFile (iFd, "", 1, sData);
		ulUnknownB[iLoop] = ReadFromFile (iFd, "", 1, sData);
		ulUnknownC[iLoop] = ReadFromFile (iFd, "", 1, sData);
		ulUnknownD[iLoop] = ReadFromFile (iFd, "", 1, sData);
	}

	/*** 0x00 0x00 ***/
	ReadFromFile (iFd, "", 2, sData);

	/*** front types ***/
	ulFrontTypes = ReadFromFile (iFd, "", 2, sData);
	for (iLoop = 1; iLoop <= (int)ulFrontTypes; iLoop++)
	{
		ulFrontTypeA[iLoop] = ReadFromFile (iFd, "", 1, sData);
		ulFrontTypeB[iLoop] = ReadFromFile (iFd, "", 1, sData);
		ulFrontTypeNr[iLoop] = ReadFromFile (iFd, "", 1, sData);
	}

	/*** front ***/
	ulNrFront = ReadFromFile (iFd, "", 2, sData);
	for (iLoop = 1; iLoop <= (int)ulNrFront; iLoop++)
	{
		ulFrontX[iLoop] = ReadFromFile (iFd, "", 2, sData);
		ulFrontY[iLoop] = ReadFromFile (iFd, "", 2, sData);
		ulFrontType[iLoop] = ReadFromFile (iFd, "", 1, sData);
		ulFrontA[iLoop] = ReadFromFile (iFd, "", 1, sData);
		ulFrontB[iLoop] = ReadFromFile (iFd, "", 1, sData);
	}

	/*** prince ***/
	ulPrinceX = ReadFromFile (iFd, "", 2, sData);
	ulPrinceY = ReadFromFile (iFd, "", 2, sData);

	/*** Hover starts more or less where the prince is. ***/
	iHoverRow = round (ulPrinceY / 24) + 1;
	if (iHoverRow > (int)ulHeight) { iHoverRow = (int)ulHeight; }
	if (iHoverRow < 1) { iHoverRow = 1; }
	iHoverColumn = round (ulPrinceX / 16) + 1;
	if (iHoverColumn > (int)ulWidth) { iHoverColumn = (int)ulWidth; }
	if (iHoverColumn < 1) { iHoverColumn = 1; }

	/*** exit trigger ***/
	ulExitTriggerX = ReadFromFile (iFd, "", 2, sData);
	ulExitTriggerY = ReadFromFile (iFd, "", 2, sData);

	/*** save ***/
	ulSaveTriggerX = ReadFromFile (iFd, "", 2, sData);
	ulSaveTriggerY = ReadFromFile (iFd, "", 2, sData);

	/*** entrance image ***/
	ulEntranceImageX = ReadFromFile (iFd, "", 2, sData);
	ulEntranceImageY = ReadFromFile (iFd, "", 2, sData);

	/*** exit image ***/
	ulExitImageX = ReadFromFile (iFd, "", 2, sData);
	ulExitImageY = ReadFromFile (iFd, "", 2, sData);

	/*** chompers ***/
	ulNrChompers = ReadFromFile (iFd, "", 2, sData);
	for (iLoop = 1; iLoop <= (int)ulNrChompers; iLoop++)
	{
		ulChomperX[iLoop] = ReadFromFile (iFd, "", 2, sData);
		ulChomperY[iLoop] = ReadFromFile (iFd, "", 2, sData);
		ulChomperA[iLoop] = ReadFromFile (iFd, "", 1, sData);
	}

	/*** spikes ***/
	ulNrSpikes = ReadFromFile (iFd, "", 2, sData);
	for (iLoop = 1; iLoop <= (int)ulNrSpikes; iLoop++)
	{
		ulSpikeX[iLoop] = ReadFromFile (iFd, "", 2, sData);
		ulSpikeY[iLoop] = ReadFromFile (iFd, "", 2, sData);
		ulSpikeRight[iLoop] = ReadFromFile (iFd, "", 1, sData);
	}

	/*** gates ***/
	ulNrGates = ReadFromFile (iFd, "", 2, sData);
	for (iLoop = 1; iLoop <= (int)ulNrGates; iLoop++)
	{
		ulGateX[iLoop] = ReadFromFile (iFd, "", 2, sData);
		ulGateY[iLoop] = ReadFromFile (iFd, "", 2, sData);
		ulGateTimeOpen[iLoop] = ReadFromFile (iFd, "", 2, sData);
	}

	/*** raise ***/
	ulNrRaise = ReadFromFile (iFd, "", 2, sData);
	for (iLoop = 1; iLoop <= (int)ulNrRaise; iLoop++)
	{
		ulRaiseGate[iLoop] = ReadFromFile (iFd, "", 2, sData);
		ulRaiseX[iLoop] = ReadFromFile (iFd, "", 2, sData);
		ulRaiseY[iLoop] = ReadFromFile (iFd, "", 2, sData);
	}

	/*** guards ***/
	ulNrGuards = ReadFromFile (iFd, "", 2, sData);
	for (iLoop = 1; iLoop <= (int)ulNrGuards; iLoop++)
	{
		ulGuardX[iLoop] = ReadFromFile (iFd, "", 2, sData);
		ulGuardY[iLoop] = ReadFromFile (iFd, "", 2, sData);
		ulGuardDir[iLoop] = ReadFromFile (iFd, "", 1, sData);
		ulGuardHP[iLoop] = ReadFromFile (iFd, "", 1, sData);
		ulGuardA[iLoop] = ReadFromFile (iFd, "", 1, sData);
		ulGuardB[iLoop] = ReadFromFile (iFd, "", 1, sData);
		ulGuardC[iLoop] = ReadFromFile (iFd, "", 1, sData);
		ulGuardD[iLoop] = ReadFromFile (iFd, "", 1, sData);
		ulGuardE[iLoop] = ReadFromFile (iFd, "", 1, sData);
		ulGuardF[iLoop] = ReadFromFile (iFd, "", 1, sData);
	}

	/*** potions ***/
	ulNrPotions = ReadFromFile (iFd, "", 2, sData);
	for (iLoop = 1; iLoop <= (int)ulNrPotions; iLoop++)
	{
		ulPotionType[iLoop] = ReadFromFile (iFd, "", 1, sData);
		ulPotionX[iLoop] = ReadFromFile (iFd, "", 2, sData);
		ulPotionY[iLoop] = ReadFromFile (iFd, "", 2, sData);
	}

	/*** loose ***/
	ulNrLoose = ReadFromFile (iFd, "", 2, sData);
	for (iLoop = 1; iLoop <= (int)ulNrLoose; iLoop++)
	{
		ulLooseX[iLoop] = ReadFromFile (iFd, "", 2, sData);
		ulLooseY[iLoop] = ReadFromFile (iFd, "", 2, sData);
		ulLooseRight[iLoop] = ReadFromFile (iFd, "", 1, sData);
	}

	/*** 0x00 0x00 ***/
	ReadFromFile (iFd, "", 2, sData);

	/*** text ***/
	ulNrText = ReadFromFile (iFd, "", 2, sData);
	ReadFromFile (iFd, "", (int)ulNrText, sText);

	/*** Converting sText to arTextLine. ***/
	for (iLoop = 1; iLoop <= MAX_LINES; iLoop++)
		{ snprintf (arTextLine[iLoop], MAX_LINECHARS, "%s", ""); }
	iNrLines = 1;
	for (iLoop = 0; iLoop < (int)ulNrText; iLoop++)
	{
		if (sText[iLoop] != '\\')
		{
			snprintf (sTempLine, MAX_LINECHARS, "%s%c",
				arTextLine[iNrLines], sText[iLoop]);
			snprintf (arTextLine[iNrLines], MAX_LINECHARS, "%s", sTempLine);
		} else {
			snprintf (sTempLine, MAX_LINECHARS, "%s%c",
				arTextLine[iNrLines], '\0');
			snprintf (arTextLine[iNrLines], MAX_LINECHARS, "%s", sTempLine);
			iNrLines++;
		}
	}
	iNrLines--;

	if (iDebug == 1)
	{
		printf ("[ INFO ] # Back: %lux%lu\n", ulWidth, ulHeight);
		for (iRowLoop = 1; iRowLoop <= (int)ulHeight; iRowLoop++)
		{
			for (iColumnLoop = 1; iColumnLoop <= (int)ulWidth; iColumnLoop++)
				{ printf ("%02X ", (int)ulBack[iRowLoop][iColumnLoop]); }
			printf ("\n");
		}
		printf ("[ INFO ] # Unknown: %lu\n", ulNrUnknown);
		for (iLoop = 1; iLoop <= (int)ulNrUnknown; iLoop++)
		{
			printf ("(Unknown) A: %lu, B: %lu, C: %lu, D: %lu\n",
				ulUnknownA[iLoop], ulUnknownB[iLoop],
				ulUnknownC[iLoop], ulUnknownD[iLoop]);
		}
		printf ("[ INFO ] Front types: %lu\n", ulFrontTypes);
		for (iLoop = 1; iLoop <= (int)ulFrontTypes; iLoop++)
		{
			printf ("(Front type) A: %lu, B: %lu, Nr: %lu\n",
				ulFrontTypeA[iLoop], ulFrontTypeB[iLoop], ulFrontTypeNr[iLoop]);
		}
		printf ("[ INFO ] # Front: %lu\n", ulNrFront);
		for (iLoop = 1; iLoop <= (int)ulNrFront; iLoop++)
		{
			printf ("(");
			switch (ulFrontTypeNr[ulFrontType[iLoop] + 1])
			{
				case 0x30: printf ("Torch"); break;
				case 0x31: printf ("Pillar Front"); break;
				case 0x32: printf ("Skeleton"); break;
				case 0x33: printf ("Wall Top Left Slash"); break;
				case 0x34: printf ("Wall Top Left Dot"); break;
				case 0x35: printf ("Wall Bottom Left"); break;
				case 0x36: printf ("Floor Climbable"); break;
				default:
					printf ("%lu (0x%02X)", /*** No WARN or \n. ***/
						ulFrontTypeNr[ulFrontType[iLoop] + 1],
						(int)ulFrontTypeNr[ulFrontType[iLoop] + 1]);
					break;
			}
			printf (") X: %lu, Y: %lu, type: %lu, A: %lu, B: %lu\n",
				ulFrontX[iLoop], ulFrontY[iLoop], ulFrontType[iLoop],
				ulFrontA[iLoop], ulFrontB[iLoop]);
		}
		printf ("[ INFO ] Prince X: %lu, Y: %lu\n", ulPrinceX, ulPrinceY);
		printf ("[ INFO ] Exit trigger X: %lu, Y: %lu\n",
			ulExitTriggerX, ulExitTriggerY);
		printf ("[ INFO ] Save trigger X: %lu, Y: %lu\n",
			ulSaveTriggerX, ulSaveTriggerY);
		printf ("[ INFO ] Entrance image X: %lu, Y: %lu\n",
			ulEntranceImageX, ulEntranceImageY);
		printf ("[ INFO ] Exit image X: %lu, Y: %lu\n",
			ulExitImageX, ulExitImageY);
		printf ("[ INFO ] # Chompers: %lu\n", ulNrChompers);
		for (iLoop = 1; iLoop <= (int)ulNrChompers; iLoop++)
		{
			printf ("(Chomper) X: %lu, Y: %lu, A: %lu\n",
				ulChomperX[iLoop], ulChomperY[iLoop], ulChomperA[iLoop]);
		}
		printf ("[ INFO ] # Spikes: %lu\n", ulNrSpikes);
		for (iLoop = 1; iLoop <= (int)ulNrSpikes; iLoop++)
		{
			printf ("(Spikes) X: %lu, Y: %lu, right: %lu\n",
				ulSpikeX[iLoop], ulSpikeY[iLoop], ulSpikeRight[iLoop]);
		}
		printf ("[ INFO ] # Gates: %lu\n", ulNrGates);
		for (iLoop = 1; iLoop <= (int)ulNrGates; iLoop++)
		{
			printf ("(Gate) X: %lu, Y: %lu, time open: %lu\n",
				ulGateX[iLoop], ulGateY[iLoop], ulGateTimeOpen[iLoop]);
		}
		printf ("[ INFO ] # Raise: %lu\n", ulNrRaise);
		for (iLoop = 1; iLoop <= (int)ulNrRaise; iLoop++)
		{
			printf ("(Raise) Gate: %lu, X: %lu, Y: %lu\n",
				ulRaiseGate[iLoop], ulRaiseX[iLoop], ulRaiseY[iLoop]);
		}
		printf ("[ INFO ] # Guards: %lu\n", ulNrGuards);
		for (iLoop = 1; iLoop <= (int)ulNrGuards; iLoop++)
		{
			printf ("(Guard) X: %lu, Y: %lu, dir: %lu, HP: %lu,"
				" A: %lu, B: %lu, C: %lu, D: %lu, E: %lu, F: %lu\n",
				ulGuardX[iLoop], ulGuardY[iLoop], ulGuardDir[iLoop], ulGuardHP[iLoop],
				ulGuardA[iLoop], ulGuardB[iLoop], ulGuardC[iLoop],
				ulGuardD[iLoop], ulGuardE[iLoop], ulGuardF[iLoop]);
		}
		printf ("[ INFO ] # Potions (inc. save lamp anim.): %lu\n", ulNrPotions);
		for (iLoop = 1; iLoop <= (int)ulNrPotions; iLoop++)
		{
			printf ("(Potion) Type: %lu, X: %lu, Y: %lu\n",
				ulPotionType[iLoop], ulPotionX[iLoop], ulPotionY[iLoop]);
		}
		printf ("[ INFO ] # Loose: %lu\n", ulNrLoose);
		for (iLoop = 1; iLoop <= (int)ulNrLoose; iLoop++)
		{
			printf ("(Loose) X: %lu, Y: %lu, right: %lu\n",
				ulLooseX[iLoop], ulLooseY[iLoop], ulLooseRight[iLoop]);
		}
		printf ("[ INFO ] Text: %lu chars; %i lines\n", ulNrText, iNrLines);
		for (iLoop = 1; iLoop <= iNrLines; iLoop++)
		{
			printf ("(Text) >%s<\n", arTextLine[iLoop]);
		}
	}

	/*** Defaults. ***/
	iXPosDragOffset = 0;
	iYPosDragOffset = 0;
}
/*****************************************************************************/
void InitScreenAction (char *sAction)
/*****************************************************************************/
{
	int iDelXPosOld, iDelYPosOld;
	int iDelXOld, iDelYOld;
	int iX, iY;

	if (strcmp (sAction, "in") == 0)
	{
		if (iZoom < MAX_ZOOM)
		{
			iZoom++;
			PlaySound ("wav/screen2or3.wav");
		}
	}

	if (strcmp (sAction, "out") == 0)
	{
		if (iZoom > 1)
		{
			iZoom--;
			PlaySound ("wav/screen2or3.wav");
		}
	}

	if (strcmp (sAction, "del") == 0)
	{
		iXYNear = 10000;
		iDelXOld = iDelX;
		iDelYOld = iDelY;
		iDelXPosOld = iDelXPos;
		iDelYPosOld = iDelYPos;

		iX = (iXPos - MapStartX()) / iZoom;
		iY = (iYPos - MapStartY()) / iZoom;

		DelWhat (1, iX, iY); /*** front ***/
		DelWhat (2, iX, iY); /*** chompers ***/
		DelWhat (3, iX, iY); /*** spikes ***/
		DelWhat (4, iX, iY); /*** gates ***/
		DelWhat (5, iX, iY); /*** raise ***/
		DelWhat (6, iX, iY); /*** guards ***/
		DelWhat (7, iX, iY); /*** potions ***/
		DelWhat (8, iX, iY); /*** loose ***/

		if ((iDelXPosOld == iDelXPos) && (iDelYPosOld == iDelYPos) &&
			(iDelXOld == iDelX) && (iDelYOld == iDelY))
		{
			switch (iDelWhat)
			{
				case 1: /*** front ***/
					if (ulNrFront == 1)
					{
						snprintf (sWarning, MAX_WARNING, "%s",
							"Cannot remove: each level must have 1+ front.");
						Warning();
					} else { Del(); }
					break;
				case 2: Del(); break; /*** chompers ***/
				case 3: /*** spikes ***/
					if (ulNrSpikes == 1)
					{
						snprintf (sWarning, MAX_WARNING, "%s",
							"Cannot remove: each level must have 1+ spikes.");
						Warning();
					} else { Del(); }
					break;
				case 4: /*** gates ***/
					if (ulNrGates == 1)
					{
						snprintf (sWarning, MAX_WARNING, "%s",
							"Cannot remove: each level must have 1+ gates.");
						Warning();
					} else { Del(); }
					break;
				case 5: Del(); break; /*** raise ***/
				case 6: Del(); break; /*** guards ***/
				case 7: /*** potions ***/
					if (ulNrPotions == 1)
					{
						snprintf (sWarning, MAX_WARNING, "%s",
							"Cannot remove: each level must have 1+ potions.");
						Warning();
					} else { Del(); }
					break;
				case 8: Del(); break; /*** loose ***/
			}

			iDelX = -1;
			iDelY = -1;
		}
	}
}
/*****************************************************************************/
void InitScreen (void)
/*****************************************************************************/
{
	SDL_AudioSpec fmt;
	char sImage[MAX_IMG + 2];
	SDL_Surface *imgicon;
	int iJoyNr;
	SDL_Rect barbox;
	SDL_Event event;
	char sBack[MAX_FILE + 2];
	int iOldXPos, iOldYPos;
	const Uint8 *keystate;
	int iXJoy1, iYJoy1, iXJoy2, iYJoy2;

	/*** Used for looping. ***/
	int iBackLoop;

	if (SDL_Init (SDL_INIT_AUDIO|SDL_INIT_VIDEO|
		SDL_INIT_GAMECONTROLLER|SDL_INIT_HAPTIC) < 0)
	{
		snprintf (sError, MAX_ERROR, "Unable to init SDL: %s!", SDL_GetError());
		ErrorAndExit();
	}
	atexit (SDL_Quit);

	window = SDL_CreateWindow (EDITOR_NAME " " EDITOR_VERSION,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		(WINDOW_WIDTH) * iScale, (WINDOW_HEIGHT) * iScale, iFullscreen);
	if (window == NULL)
	{
		snprintf (sError, MAX_ERROR, "Unable to create a window: %s!",
			SDL_GetError());
		ErrorAndExit();
	}
	ascreen = SDL_CreateRenderer (window, -1, 0);
	if (ascreen == NULL)
	{
		snprintf (sError, MAX_ERROR, "Unable to set video mode: %s!",
			SDL_GetError());
		ErrorAndExit();
	}
	/*** Some people may prefer linear, but we're going old school. ***/
	SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	if (iFullscreen != 0)
	{
		SDL_RenderSetLogicalSize (ascreen, (WINDOW_WIDTH) * iScale,
			(WINDOW_HEIGHT) * iScale);
	}

	if (TTF_Init() == -1)
	{
		snprintf (sError, MAX_ERROR, "%s", "Could not initialize TTF!");
		ErrorAndExit();
	}

	LoadFonts();

	curArrow = SDL_CreateSystemCursor (SDL_SYSTEM_CURSOR_ARROW);
	curWait = SDL_CreateSystemCursor (SDL_SYSTEM_CURSOR_WAIT);
	curHand = SDL_CreateSystemCursor (SDL_SYSTEM_CURSOR_HAND);
	curText = SDL_CreateSystemCursor (SDL_SYSTEM_CURSOR_IBEAM);

	if (iNoAudio != 1)
	{
		PrIfDe ("[  OK  ] Initializing audio.\n");
		fmt.freq = 44100;
		fmt.format = AUDIO_S16;
		fmt.channels = 2;
		fmt.samples = 512;
		fmt.callback = MixAudio;
		fmt.userdata = NULL;
		if (SDL_OpenAudio (&fmt, NULL) < 0)
		{
			snprintf (sError, MAX_ERROR, "Unable to open audio: %s!",
				SDL_GetError());
			ErrorAndExit();
		}
		SDL_PauseAudio (0);
	}

	/*** icon ***/
	snprintf (sImage, MAX_IMG, "png%svarious%spophale_icon.png",
		SLASH, SLASH);
	imgicon = IMG_Load (sImage);
	if (imgicon == NULL)
	{
		printf ("[ WARN ] Could not load \"%s\": %s!\n", sImage, strerror (errno));
	} else {
		SDL_SetWindowIcon (window, imgicon);
	}

	/*** Open the first available controller. ***/
	iController = 0;
	if (iNoController != 1)
	{
		for (iJoyNr = 0; iJoyNr < SDL_NumJoysticks(); iJoyNr++)
		{
			if (SDL_IsGameController (iJoyNr))
			{
				controller = SDL_GameControllerOpen (iJoyNr);
				if (controller)
				{
					snprintf (sControllerName, MAX_CON, "%s",
						SDL_GameControllerName (controller));
					if (iDebug == 1)
					{
						printf ("[ INFO ] Found a controller \"%s\"; \"%s\".\n",
							sControllerName, SDL_GameControllerNameForIndex (iJoyNr));
					}
					joystick = SDL_GameControllerGetJoystick (controller);
					iController = 1;

					/*** Just for fun, use haptic. ***/
					if (SDL_JoystickIsHaptic (joystick))
					{
						haptic = SDL_HapticOpenFromJoystick (joystick);
						if (SDL_HapticRumbleInit (haptic) == 0)
						{
							SDL_HapticRumblePlay (haptic, 1.0, 1000);
						} else {
							printf ("[ WARN ] Could not initialize the haptic device: %s!\n",
								SDL_GetError());
						}
					} else {
						PrIfDe ("[ INFO ] The game controller is not haptic.\n");
					}
				} else {
					printf ("[ WARN ] Could not open game controller %i: %s!\n",
						iController, SDL_GetError());
				}
			}
		}
		if (iController != 1) { PrIfDe ("[ INFO ] No controller found.\n"); }
	} else {
		PrIfDe ("[ INFO ] Using keyboard and mouse.\n");
	}

	/*******************/
	/* Preload images. */
	/*******************/

	/*** Loading... ***/
	PreLoad (PNG_VARIOUS, "loading.png", &imgloading);
	ShowImage (imgloading, 0, 0, "imgloading", ascreen, iScale, 1);
	SDL_SetRenderDrawColor (ascreen, 0x22, 0x22, 0x22, SDL_ALPHA_OPAQUE);
	barbox.x = 10 * iScale;
	barbox.y = 10 * iScale;
	barbox.w = 20 * iScale;
	barbox.h = 662 * iScale;
	SDL_RenderFillRect (ascreen, &barbox);
	SDL_RenderPresent (ascreen);

	iPreLoaded = 0;
	iCurrentBarHeight = 0;
	iNrToPreLoad = 133; /*** Value can be obtained via debug mode. ***/
	SDL_SetCursor (curWait);

	/*** back ***/
	for (iBackLoop = 0; iBackLoop <= 55; iBackLoop++)
	{
		snprintf (sBack, MAX_FILE, "%02X.png", iBackLoop);
		PreLoad (PNG_BACK, sBack, &imgback[iBackLoop]);
	}
	PreLoad (PNG_VARIOUS, "back_tiles.png", &imgbacktiles);
	PreLoad (PNG_VARIOUS, "hover_back_l.png", &imghoverbackl);
	PreLoad (PNG_VARIOUS, "hover_back_l_live.png", &imghoverbackllive);

	/*** front ***/
	PreLoad (PNG_VARIOUS, "front.png", &imgfront);
	PreLoad (PNG_VARIOUS, "hover_front.png", &imghoverfront);

	/*** buttons ***/
	if (iController != 1)
	{
		PreLoad (PNG_BUTTONS, "previous_off.png", &imgprevoff);
		PreLoad (PNG_BUTTONS, "previous_on_0.png", &imgprevon_0);
		PreLoad (PNG_BUTTONS, "previous_on_1.png", &imgprevon_1);
		PreLoad (PNG_BUTTONS, "next_off.png", &imgnextoff);
		PreLoad (PNG_BUTTONS, "next_on_0.png", &imgnexton_0);
		PreLoad (PNG_BUTTONS, "next_on_1.png", &imgnexton_1);
		PreLoad (PNG_BUTTONS, "exe_off.png", &imgexeoff);
		PreLoad (PNG_BUTTONS, "exe_on_0.png", &imgexeon_0);
		PreLoad (PNG_BUTTONS, "exe_on_1.png", &imgexeon_1);
		PreLoad (PNG_BUTTONS, "OK.png", &imgok[1]);
		PreLoad (PNG_BUTTONS, "sel_OK.png", &imgok[2]);
		PreLoad (PNG_BUTTONS, "close_0.png", &imgclose[1]);
		PreLoad (PNG_BUTTONS, "close_1.png", &imgclose[2]);
		PreLoad (PNG_BUTTONS, "save_off.png", &imgsaveoff);
		PreLoad (PNG_BUTTONS, "save_on_0.png", &imgsaveon_0);
		PreLoad (PNG_BUTTONS, "save_on_1.png", &imgsaveon_1);
		PreLoad (PNG_BUTTONS, "Yes.png", &imgyes[1]);
		PreLoad (PNG_BUTTONS, "sel_Yes.png", &imgyes[2]);
		PreLoad (PNG_BUTTONS, "No.png", &imgno[1]);
		PreLoad (PNG_BUTTONS, "sel_No.png", &imgno[2]);
		PreLoad (PNG_BUTTONS, "quit_0.png", &imgquit_0);
		PreLoad (PNG_BUTTONS, "quit_1.png", &imgquit_1);
		PreLoad (PNG_BUTTONS, "Save.png", &imgsave[1]);
		PreLoad (PNG_BUTTONS, "sel_Save.png", &imgsave[2]);
		PreLoad (PNG_BUTTONS, "text_off.png", &imgtextoff);
		PreLoad (PNG_BUTTONS, "text_on_0.png", &imgtexton_0);
		PreLoad (PNG_BUTTONS, "text_on_1.png", &imgtexton_1);
		PreLoad (PNG_BUTTONS, "playtest_off.png", &imgplayoff);
		PreLoad (PNG_BUTTONS, "playtest_on_0.png", &imgplayon_0);
		PreLoad (PNG_BUTTONS, "playtest_on_1.png", &imgplayon_1);
		PreLoad (PNG_BUTTONS, "help_off.png", &imghelpoff);
		PreLoad (PNG_BUTTONS, "help_on_0.png", &imghelpon_0);
		PreLoad (PNG_BUTTONS, "help_on_1.png", &imghelpon_1);
	} else {
		PreLoad (PNG_GAMEPAD, "previous_off.png", &imgprevoff);
		PreLoad (PNG_GAMEPAD, "previous_on_0.png", &imgprevon_0);
		PreLoad (PNG_GAMEPAD, "previous_on_1.png", &imgprevon_1);
		PreLoad (PNG_GAMEPAD, "next_off.png", &imgnextoff);
		PreLoad (PNG_GAMEPAD, "next_on_0.png", &imgnexton_0);
		PreLoad (PNG_GAMEPAD, "next_on_1.png", &imgnexton_1);
		PreLoad (PNG_GAMEPAD, "exe_off.png", &imgexeoff);
		PreLoad (PNG_GAMEPAD, "exe_on_0.png", &imgexeon_0);
		PreLoad (PNG_GAMEPAD, "exe_on_1.png", &imgexeon_1);
		PreLoad (PNG_GAMEPAD, "OK.png", &imgok[1]);
		PreLoad (PNG_GAMEPAD, "sel_OK.png", &imgok[2]);
		PreLoad (PNG_GAMEPAD, "close_0.png", &imgclose[1]);
		PreLoad (PNG_GAMEPAD, "close_1.png", &imgclose[2]);
		PreLoad (PNG_GAMEPAD, "save_off.png", &imgsaveoff);
		PreLoad (PNG_GAMEPAD, "save_on_0.png", &imgsaveon_0);
		PreLoad (PNG_GAMEPAD, "save_on_1.png", &imgsaveon_1);
		PreLoad (PNG_GAMEPAD, "Yes.png", &imgyes[1]);
		PreLoad (PNG_GAMEPAD, "sel_Yes.png", &imgyes[2]);
		PreLoad (PNG_GAMEPAD, "No.png", &imgno[1]);
		PreLoad (PNG_GAMEPAD, "sel_No.png", &imgno[2]);
		PreLoad (PNG_GAMEPAD, "quit_0.png", &imgquit_0);
		PreLoad (PNG_GAMEPAD, "quit_1.png", &imgquit_1);
		PreLoad (PNG_GAMEPAD, "Save.png", &imgsave[1]);
		PreLoad (PNG_GAMEPAD, "sel_Save.png", &imgsave[2]);
		PreLoad (PNG_GAMEPAD, "text_off.png", &imgtextoff);
		PreLoad (PNG_GAMEPAD, "text_on_0.png", &imgtexton_0);
		PreLoad (PNG_GAMEPAD, "text_on_1.png", &imgtexton_1);
		PreLoad (PNG_GAMEPAD, "playtest_off.png", &imgplayoff);
		PreLoad (PNG_GAMEPAD, "playtest_on_0.png", &imgplayon_0);
		PreLoad (PNG_GAMEPAD, "playtest_on_1.png", &imgplayon_1);
		PreLoad (PNG_GAMEPAD, "help_off.png", &imghelpoff);
		PreLoad (PNG_GAMEPAD, "help_on_0.png", &imghelpon_0);
		PreLoad (PNG_GAMEPAD, "help_on_1.png", &imghelpon_1);
	}

	/*** various ***/
	PreLoad (PNG_VARIOUS, "black.png", &imgblack);
	PreLoad (PNG_VARIOUS, "interface.png", &imginterface);
	PreLoad (PNG_VARIOUS, "exe.png", &imgexe);
	PreLoad (PNG_VARIOUS, "text.png", &imgtext);
	PreLoad (PNG_VARIOUS, "hover_text.png", &imghovertext);
	PreLoad (PNG_VARIOUS, "help.png", &imghelp);
	PreLoad (PNG_VARIOUS, "hover_back_s.png", &imghoverbacks);
	PreLoad (PNG_VARIOUS, "entrance.png", &imgentrance);
	PreLoad (PNG_VARIOUS, "exit.png", &imgexit);
	PreLoad (PNG_VARIOUS, "prince_r.png", &imgprincer);
	PreLoad (PNG_VARIOUS, "exit_trigger.png", &imgexittrigger);
	PreLoad (PNG_VARIOUS, "save_trigger.png", &imgsavetrigger);
	PreLoad (PNG_VARIOUS, "chomper.png", &imgchomper);
	PreLoad (PNG_VARIOUS, "spikes_0.png", &imgspikes0);
	PreLoad (PNG_VARIOUS, "spikes_1.png", &imgspikes1);
	PreLoad (PNG_VARIOUS, "gate.png", &imggate);
	PreLoad (PNG_VARIOUS, "raise.png", &imgraise);
	PreLoad (PNG_VARIOUS, "guard_r.png", &imgguardr);
	PreLoad (PNG_VARIOUS, "guard_l.png", &imgguardl);
	PreLoad (PNG_VARIOUS, "potion_0.png", &imgpotion0);
	PreLoad (PNG_VARIOUS, "potion_1.png", &imgpotion1);
	PreLoad (PNG_VARIOUS, "potion_2.png", &imgpotion2);
	PreLoad (PNG_VARIOUS, "potion_3.png", &imgpotion3);
	PreLoad (PNG_VARIOUS, "loose_0.png", &imgloose0);
	PreLoad (PNG_VARIOUS, "loose_1.png", &imgloose1);
	PreLoad (PNG_VARIOUS, "faded.png", &imgfaded);
	PreLoad (PNG_VARIOUS, "popup_yn.png", &imgpopup_yn);
	PreLoad (PNG_VARIOUS, "emulator.png", &imgemulator);
	PreLoad (PNG_VARIOUS, "delete.png", &imgdelete);
	PreLoad (PNG_VARIOUS, "statusbar_sprite.png", &imgstatusbarsprite);
	PreLoad (PNG_VARIOUS, "popup.png", &imgpopup);
	PreLoad (PNG_VARIOUS, "chk_black.png", &imgchkb);

	/*** front ***/
	PreLoad (PNG_FRONT, "torch_sprite.png", &imgtorchsprite);
	PreLoad (PNG_FRONT, "pillar_front.png", &imgpillarfront);
	PreLoad (PNG_FRONT, "skeleton.png", &imgskeleton);
	PreLoad (PNG_FRONT, "wall_top_left_slash.png", &imgwalltopleftslash);
	PreLoad (PNG_FRONT, "wall_top_left_dot.png", &imgwalltopleftdot);
	PreLoad (PNG_FRONT, "floor_climbable.png", &imgfloorclimbable);
	PreLoad (PNG_FRONT, "wall_bottom_left.png", &imgwallbottomleft);

	if (iDebug == 1)
		{ printf ("[ INFO ] Preloaded images: %i\n", iPreLoaded); }
	SDL_SetCursor (curArrow);

	/*** Defaults. ***/
	iCurLevel = iStartLevel;
	iDownAt = 0;
	iDraggingMap = 0;
	iFlameFrame = 1;
	oldticks = 0;
	oldticksf = 0;

	ShowScreen();
	InitPopUp();
	while (1)
	{
		if (iNoAnim == 0)
		{
			/*** This is for the animation; ~30 fps (1000/33). ***/
			newticks = SDL_GetTicks();
			if (newticks > oldticks + REFRESH_GAME)
			{
				ShowScreen();
				oldticks = newticks;
			}
		}

		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_CONTROLLERBUTTONDOWN:
					/*** Nothing for now. ***/
					break;
				case SDL_CONTROLLERBUTTONUP:
					switch (event.cbutton.button)
					{
						case SDL_CONTROLLER_BUTTON_A:
							if (iDraggingMoved == 0) { ChangeBack(); }
							break;
						case SDL_CONTROLLER_BUTTON_B: Quit(); break;
						case SDL_CONTROLLER_BUTTON_X: Text(); break;
						case SDL_CONTROLLER_BUTTON_Y: EXE(); break;
						case SDL_CONTROLLER_BUTTON_BACK: Help(); break;
						case SDL_CONTROLLER_BUTTON_GUIDE: LevelSave(); break;
						case SDL_CONTROLLER_BUTTON_START: Playtest(); break;
						case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: PrevLevel(); break;
						case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: NextLevel(); break;
						case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
							/*** move active back marker ***/
							if (iHoverColumn > 1) { iHoverColumn--; }
							break;
						case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
							/*** move active back marker ***/
							if (iHoverColumn < (int)ulWidth) { iHoverColumn++; }
							break;
						case SDL_CONTROLLER_BUTTON_DPAD_UP:
							/*** move active back marker ***/
							if (iHoverRow > 1) { iHoverRow--; }
							break;
						case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
							/*** move active back marker ***/
							if (iHoverRow < (int)ulHeight) { iHoverRow++; }
							break;
					}
					ShowScreen();
					break;
				case SDL_CONTROLLERAXISMOTION: /*** triggers and analog sticks ***/
					iXJoy1 = SDL_JoystickGetAxis (joystick, 0);
					iYJoy1 = SDL_JoystickGetAxis (joystick, 1);
					iXJoy2 = SDL_JoystickGetAxis (joystick, 3);
					iYJoy2 = SDL_JoystickGetAxis (joystick, 4);
					if ((iXJoy1 < -30000) || (iXJoy2 < -30000)) /*** left ***/
					{
						if ((SDL_GetTicks() - joyleft) > 300)
						{
							/*** 'drag' ***/
							iXPosDragOffset+=100;
							joyleft = SDL_GetTicks();
						}
					}
					if ((iXJoy1 > 30000) || (iXJoy2 > 30000)) /*** right ***/
					{
						if ((SDL_GetTicks() - joyright) > 300)
						{
							/*** 'drag' ***/
							iXPosDragOffset-=100;
							joyright = SDL_GetTicks();
						}
					}
					if ((iYJoy1 < -30000) || (iYJoy2 < -30000)) /*** up ***/
					{
						if ((SDL_GetTicks() - joyup) > 300)
						{
							/*** 'drag' ***/
							iYPosDragOffset+=100;
							joyup = SDL_GetTicks();
						}
					}
					if ((iYJoy1 > 30000) || (iYJoy2 > 30000)) /*** down ***/
					{
						if ((SDL_GetTicks() - joydown) > 300)
						{
							/*** 'drag' ***/
							iYPosDragOffset-=100;
							joydown = SDL_GetTicks();
						}
					}
					if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)
					{
						if ((SDL_GetTicks() - trigleft) > 300)
						{
							InitScreenAction ("out");
							trigleft = SDL_GetTicks();
						}
					}
					if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
					{
						if ((SDL_GetTicks() - trigright) > 300)
						{
							InitScreenAction ("in");
							trigright = SDL_GetTicks();
						}
					}
					ShowScreen();
					break;
				case SDL_KEYDOWN: /*** https://wiki.libsdl.org/SDL2/SDL_Keycode ***/
					switch (event.key.keysym.sym)
					{
						case SDLK_MINUS:
						case SDLK_KP_MINUS:
							PrevLevel();
							break;
						case SDLK_KP_PLUS:
						case SDLK_EQUALS:
							NextLevel();
							break;
						case SDLK_F1: Help(); break;
						case SDLK_1: case SDLK_KP_1: SetZoom (1); break;
						case SDLK_2: case SDLK_KP_2: SetZoom (2); break;
						case SDLK_3: case SDLK_KP_3: SetZoom (3); break;
						case SDLK_4: case SDLK_KP_4: SetZoom (4); break;
						case SDLK_5: case SDLK_KP_5: SetZoom (5); break;
						case SDLK_6: case SDLK_KP_6: SetZoom (6); break;
						case SDLK_7: case SDLK_KP_7: SetZoom (7); break;
						case SDLK_s: LevelSave(); break;
						case SDLK_KP_ENTER:
						case SDLK_RETURN:
						case SDLK_SPACE:
							if ((event.key.keysym.mod & KMOD_LALT) ||
								(event.key.keysym.mod & KMOD_RALT))
							{
								Zoom (1);
							} else {
								if (iDraggingMap == 0) { ChangeBack(); }
							}
							break;
						case SDLK_f: Zoom (1); break;
						case SDLK_z: Zoom (0); break;
						case SDLK_d: Playtest(); break;
						case SDLK_QUOTE:
							ulBack[iHoverRow][iHoverColumn] = iLastBack;
							PlaySound ("wav/ok_close.wav");
							iChanged++;
							break;
						case SDLK_ESCAPE:
						case SDLK_q:
							Quit();
							break;
						case SDLK_SLASH:
							if ((event.key.keysym.mod & KMOD_LSHIFT) ||
								(event.key.keysym.mod & KMOD_RSHIFT))
							{
								Help();
							} else {
								ClearLevel();
							}
							break;
						case SDLK_t: Text(); break;
						case SDLK_e:
						case SDLK_F2:
							EXE();
							break;
						case SDLK_INSERT:
							if (iDraggingMoved == 0) { ChangeFront(); }
							break;
						case SDLK_DELETE: InitScreenAction ("del"); break;
						case SDLK_LEFT:
							if ((event.key.keysym.mod & KMOD_LSHIFT) ||
								(event.key.keysym.mod & KMOD_RSHIFT))
							{
								/*** 'drag' ***/
								iXPosDragOffset+=100;
							} else {
								/*** move active back marker ***/
								if (iHoverColumn > 1) { iHoverColumn--; }
							}
							break;
						case SDLK_RIGHT:
							if ((event.key.keysym.mod & KMOD_LSHIFT) ||
								(event.key.keysym.mod & KMOD_RSHIFT))
							{
								/*** 'drag' ***/
								iXPosDragOffset-=100;
							} else {
								/*** move active back marker ***/
								if (iHoverColumn < (int)ulWidth) { iHoverColumn++; }
							}
							break;
						case SDLK_UP:
							if ((event.key.keysym.mod & KMOD_LSHIFT) ||
								(event.key.keysym.mod & KMOD_RSHIFT))
							{
								/*** 'drag' ***/
								iYPosDragOffset+=100;
							} else {
								/*** move active back marker ***/
								if (iHoverRow > 1) { iHoverRow--; }
							}
							break;
						case SDLK_DOWN:
							if ((event.key.keysym.mod & KMOD_LSHIFT) ||
								(event.key.keysym.mod & KMOD_RSHIFT))
							{
								/*** 'drag' ***/
								iYPosDragOffset-=100;
							} else {
								/*** move active back marker ***/
								if (iHoverRow < (int)ulHeight) { iHoverRow++; }
							}
							break;
						case SDLK_h:
							if (iHideFront == 0)
								{ iHideFront = 1; } else { iHideFront = 0; }
							PlaySound ("wav/check_box.wav");
							break;
					}
					ShowScreen();
					break;
				case SDL_MOUSEMOTION:
					iOldXPos = iXPos;
					iOldYPos = iYPos;
					iXPos = event.motion.x;
					iYPos = event.motion.y;
					if ((iOldXPos == iXPos) && (iOldYPos == iYPos)) { break; }
					UpdateHover();

					/*** Emulator information. ***/
					if (InArea (660, 2, 660 + 25, 2 + 25) == 1)
						{ iEmulator = 1; } else { iEmulator = 0; }

					if (InArea (MAP_LEFT, MAP_TOP, MAP_LEFT + MAP_WIDTH,
						MAP_TOP + MAP_HEIGHT) == 0) /*** (not) map ***/
					{
						/*** Stop map dragging. ***/
						iDraggingMap = 0;
						SDL_SetCursor (curArrow);
					}

					if (iDraggingMap == 1)
					{
						iXPosDragOffset += (iXPos - iXPosDragStart);
						iYPosDragOffset += (iYPos - iYPosDragStart);
						iXPosDragStart = iXPos;
						iYPosDragStart = iYPos;

						iDraggingMoved = 1;
						SDL_SetCursor (curHand);
					}

					ShowScreen();
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (InArea (2, 2, 2 + 25, 2 + 25) == 1) /*** prev level ***/
						{ iDownAt = 1; }
					if (InArea (52, 2, 52 + 25, 2 + 25) == 1) /*** next level ***/
						{ iDownAt = 2; }
					if (InArea (610, 2, 610 + 25, 2 + 25) == 1) /*** exe ***/
						{ iDownAt = 3; }
					if (InArea (635, 2, 635 + 25, 2 + 25) == 1) /*** text ***/
						{ iDownAt = 4; }
					if (InArea (660, 2, 660 + 25, 2 + 25) == 1) /*** playtest ***/
						{ iDownAt = 5; }
					if (InArea (685, 2, 685 + 25, 2 + 25) == 1) /*** help ***/
						{ iDownAt = 6; }
					if (InArea (2, 655, 2 + 25, 655 + 25) == 1) /*** save ***/
						{ iDownAt = 7; }
					if (InArea (685, 655, 685 + 25, 655 + 25) == 1) /*** quit ***/
						{ iDownAt = 8; }
					if (InArea (MAP_LEFT, MAP_TOP, MAP_LEFT + MAP_WIDTH,
						MAP_TOP + MAP_HEIGHT) == 1) /*** map ***/
					{
						/*** Start map dragging. ***/
						if (iDraggingMap == 0)
						{
							iXPosDragStart = iXPos;
							iYPosDragStart = iYPos;
							iDraggingMap = 1;

							iDraggingMoved = 0;
						}
					}
					ShowScreen();
					break;
				case SDL_MOUSEBUTTONUP:
					iDownAt = 0;

					/*** Stop map dragging. ***/
					if (iDraggingMap == 1)
					{
						iDraggingMap = 0;
						SDL_SetCursor (curArrow);
					}

					if (event.button.button == 1)
					{
						if (InArea (2, 2, 2 + 25, 2 + 25) == 1) /*** prev level ***/
						{
							PrevLevel();
							break; /*** Exit case SDL_MOUSEBUTTONUP. ***/
						}
						if (InArea (52, 2, 52 + 25, 2 + 25) == 1) /*** next level ***/
						{
							NextLevel();
							break; /*** Exit case SDL_MOUSEBUTTONUP. ***/
						}
						if (InArea (610, 2, 610 + 25, 2 + 25) == 1) /*** exe ***/
						{
							EXE();
							break; /*** Exit case SDL_MOUSEBUTTONUP. ***/
						}
						if (InArea (635, 2, 635 + 25, 2 + 25) == 1) /*** text ***/
						{
							Text();
							break; /*** Exit case SDL_MOUSEBUTTONUP. ***/
						}
						if (InArea (660, 2, 660 + 25, 2 + 25) == 1) /*** playtest ***/
							{ Playtest(); }
						if (InArea (685, 2, 685 + 25, 2 + 25) == 1) /*** help ***/
						{
							Help();
							break; /*** Exit case SDL_MOUSEBUTTONUP. ***/
						}
						if (InArea (2, 655, 2 + 25, 655 + 25) == 1) /*** save ***/
							{ LevelSave(); }
						if (InArea (685, 655, 685 + 25, 655 + 25) == 1) /*** quit ***/
							{ Quit(); }
						if (InArea (MAP_LEFT, MAP_TOP, MAP_LEFT + MAP_WIDTH,
							MAP_TOP + MAP_HEIGHT) == 1) /*** map ***/
						{
							if (iDraggingMoved == 0)
							{
								keystate = SDL_GetKeyboardState (NULL);
								if ((keystate[SDL_SCANCODE_LSHIFT]) ||
									(keystate[SDL_SCANCODE_RSHIFT]))
								{
									ulBack[iHoverRow][iHoverColumn] = iLastBack;
									PlaySound ("wav/ok_close.wav");
									iChanged++;
								} else {
									ChangeBack();
								}
							}
						}
						if (InArea (108, 7, 108 + 14, 7 + 14) == 1) /*** hide front t. ***/
						{
							if (iHideFront == 0)
								{ iHideFront = 1; } else { iHideFront = 0; }
							PlaySound ("wav/check_box.wav");
						}
					}
					if (event.button.button == 2) /*** middle mouse button ***/
					{
						if (InArea (MAP_LEFT, MAP_TOP, MAP_LEFT + MAP_WIDTH,
							MAP_TOP + MAP_HEIGHT) == 1) /*** map ***/
						{
							InitScreenAction ("del");
						}
					}
					if (event.button.button == 3) /*** right mouse button ***/
					{
						if (InArea (MAP_LEFT, MAP_TOP, MAP_LEFT + MAP_WIDTH,
							MAP_TOP + MAP_HEIGHT) == 1) /*** map ***/
						{
							if (iDraggingMoved == 0) { ChangeFront(); }
						}
					}

					ShowScreen();
					break;
				case SDL_MOUSEWHEEL:
					if (event.wheel.y > 0) /*** scroll wheel up ***/
						{ InitScreenAction ("in"); }
					if (event.wheel.y < 0) /*** scroll wheel down ***/
						{ InitScreenAction ("out"); }
					ShowScreen();
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_EXPOSED)
						{ ShowScreen(); } break;
				case SDL_QUIT:
					Quit(); break;
				default: break;
			}
		}

		/*** prevent CPU eating ***/
		gamespeed = REFRESH_PROG;
		while ((SDL_GetTicks() - looptime) < gamespeed)
		{
			SDL_Delay (10);
		}
		looptime = SDL_GetTicks();
	}
}
/*****************************************************************************/
void ShowFrontTiles (void)
/*****************************************************************************/
{
	/*** Used for looping. ***/
	int iLoop;

	/*** entrance image ***/
	ShowImage (imgentrance,
		MapStartX() + (ulEntranceImageX * iZoom) - (19 * iZoom),
		MapStartY() + (ulEntranceImageY * iZoom) - (33 * iZoom),
		"imgentrance", ascreen, iZoom, 0);

	/*** exit image ***/
	ShowImage (imgexit,
		MapStartX() + (ulExitImageX * iZoom) - (19 * iZoom),
		MapStartY() + (ulExitImageY * iZoom) - (33 * iZoom),
		"imgexit", ascreen, iZoom, 0);

	/*** prince ***/
	ShowImage (imgprincer,
		MapStartX() + (ulPrinceX * iZoom) - (3 * iZoom),
		MapStartY() + (ulPrinceY * iZoom) - (26 * iZoom),
		"imgprincer", ascreen, iZoom, 0);

	/*** exit trigger ***/
	ShowImage (imgexittrigger,
		MapStartX() + (ulExitTriggerX * iZoom) - (9 * iZoom),
		MapStartY() + (ulExitTriggerY * iZoom) - (9 * iZoom),
		"imgexittrigger", ascreen, iZoom, 0);

	/*** save trigger ***/
	ShowImage (imgsavetrigger,
		MapStartX() + (ulSaveTriggerX * iZoom) - (9 * iZoom),
		MapStartY() + (ulSaveTriggerY * iZoom) - (9 * iZoom),
		"imgsavetrigger", ascreen, iZoom, 0);

	/*** chompers ***/
	for (iLoop = 1; iLoop <= (int)ulNrChompers; iLoop++)
	{
		ShowImage (imgchomper,
			MapStartX() + (ulChomperX[iLoop] * iZoom) - (8 * iZoom),
			MapStartY() + (ulChomperY[iLoop] * iZoom) - (46 * iZoom),
			"imgchomper", ascreen, iZoom, 0);
	}

	/*** spikes ***/
	for (iLoop = 1; iLoop <= (int)ulNrSpikes; iLoop++)
	{
		switch (ulSpikeRight[iLoop])
		{
			case 0:
				ShowImage (imgspikes0,
					MapStartX() + (ulSpikeX[iLoop] * iZoom) - (10 * iZoom),
					MapStartY() + (ulSpikeY[iLoop] * iZoom) - (24 * iZoom),
					"imgspikes0", ascreen, iZoom, 0);
				break;
			case 1:
				ShowImage (imgspikes1,
					MapStartX() + (ulSpikeX[iLoop] * iZoom) - (10 * iZoom),
					MapStartY() + (ulSpikeY[iLoop] * iZoom) - (24 * iZoom),
					"imgspikes1", ascreen, iZoom, 0);
				break;
			default:
				printf ("[ WARN ] Unknown spikes right: %lu!\n", ulSpikeRight[iLoop]);
				break;
		}
	}

	/*** gates ***/
	for (iLoop = 1; iLoop <= (int)ulNrGates; iLoop++)
	{
		ShowImage (imggate,
			MapStartX() + (ulGateX[iLoop] * iZoom) - (6 * iZoom),
			MapStartY() + (ulGateY[iLoop] * iZoom) - (46 * iZoom),
			"imggate", ascreen, iZoom, 0);
		snprintf (arText[0], MAX_TEXT, "%i", iLoop - 1);
		DisplayText (MapStartX() + (ulGateX[iLoop] * iZoom) - (1 * iZoom),
			MapStartY() + (ulGateY[iLoop] * iZoom) - (42 * iZoom),
			(10 * iZoom) / iScale, 1, color_wh, 0);
	}

	/*** raise ***/
	for (iLoop = 1; iLoop <= (int)ulNrRaise; iLoop++)
	{
		ShowImage (imgraise,
			MapStartX() + (ulRaiseX[iLoop] * iZoom) - (14 * iZoom),
			MapStartY() + (ulRaiseY[iLoop] * iZoom) - (15 * iZoom),
			"imgraise", ascreen, iZoom, 0);
		snprintf (arText[0], MAX_TEXT, "%lu", ulRaiseGate[iLoop]);
		DisplayText (MapStartX() + (ulRaiseX[iLoop] * iZoom) - (4 * iZoom),
			MapStartY() + (ulRaiseY[iLoop] * iZoom) - (15 * iZoom),
			(10 * iZoom) / iScale, 1, color_wh, 0);
	}

	/*** guards ***/
	for (iLoop = 1; iLoop <= (int)ulNrGuards; iLoop++)
	{
		switch (ulGuardDir[iLoop])
		{
			case 0: /*** left ***/
				ShowImage (imgguardl,
					MapStartX() + (ulGuardX[iLoop] * iZoom) - (13 * iZoom),
					MapStartY() + (ulGuardY[iLoop] * iZoom) - (27 * iZoom),
					"imgguardl", ascreen, iZoom, 0);
				break;
			case 1: /*** right ***/
				ShowImage (imgguardr,
					MapStartX() + (ulGuardX[iLoop] * iZoom) - (13 * iZoom),
					MapStartY() + (ulGuardY[iLoop] * iZoom) - (27 * iZoom),
					"imgguardr", ascreen, iZoom, 0);
				break;
		}
	}

	/*** potions ***/
	for (iLoop = 1; iLoop <= (int)ulNrPotions; iLoop++)
	{
		switch (ulPotionType[iLoop])
		{
			case 0: /*** life potion ***/
				ShowImage (imgpotion0,
					MapStartX() + (ulPotionX[iLoop] * iZoom) - (4 * iZoom),
					MapStartY() + (ulPotionY[iLoop] * iZoom) - (7 * iZoom),
					"imgpotion0", ascreen, iZoom, 0);
				break;
			case 1: /*** hurt potion ***/
				ShowImage (imgpotion1,
					MapStartX() + (ulPotionX[iLoop] * iZoom) - (3 * iZoom),
					MapStartY() + (ulPotionY[iLoop] * iZoom) - (7 * iZoom),
					"imgpotion1", ascreen, iZoom, 0);
				break;
			case 2: /*** health potion ***/
				ShowImage (imgpotion2,
					MapStartX() + (ulPotionX[iLoop] * iZoom) - (3 * iZoom),
					MapStartY() + (ulPotionY[iLoop] * iZoom) - (6 * iZoom),
					"imgpotion2", ascreen, iZoom, 0);
				break;
			case 3: /*** save lamp animation ***/
				ShowImage (imgpotion3,
					MapStartX() + (ulPotionX[iLoop] * iZoom) - (11 * iZoom),
					MapStartY() + (ulPotionY[iLoop] * iZoom) - (12 * iZoom),
					"imgpotion3", ascreen, iZoom, 0);
				break;
			default:
				printf ("[ WARN ] Unknown potion: %lu\n", ulPotionType[iLoop]);
				break;
		}
	}

	/*** loose ***/
	for (iLoop = 1; iLoop <= (int)ulNrLoose; iLoop++)
	{
		switch (ulLooseRight[iLoop])
		{
			case 0:
				ShowImage (imgloose0,
					MapStartX() + (ulLooseX[iLoop] * iZoom) - (13 * iZoom),
					MapStartY() + (ulLooseY[iLoop] * iZoom) - (20 * iZoom),
					"imgloose0", ascreen, iZoom, 0);
				break;
			case 1:
				ShowImage (imgloose1,
					MapStartX() + (ulLooseX[iLoop] * iZoom) - (15 * iZoom),
					MapStartY() + (ulLooseY[iLoop] * iZoom) - (20 * iZoom),
					"imgloose1", ascreen, iZoom, 0);
				break;
			default:
				printf ("[ WARN ] Unknown loose: %lu\n", ulLooseRight[iLoop]);
				break;
		}
	}

	/*** (blue) front ***/
	for (iLoop = 1; iLoop <= (int)ulNrFront; iLoop++)
	{
		switch (ulFrontTypeNr[ulFrontType[iLoop] + 1])
		{
			case 0x30: /*** torch ***/
				ShowImage (imgtorchsprite,
					MapStartX() + (ulFrontX[iLoop] * iZoom) - (17 * iZoom),
					MapStartY() + (ulFrontY[iLoop] * iZoom) - (28 * iZoom),
					"imgtorchsprite", ascreen, iZoom, 0);
				break;
			case 0x31: /*** pillar front ***/
				ShowImage (imgpillarfront,
					MapStartX() + (ulFrontX[iLoop] * iZoom) - (1 * iZoom),
					MapStartY() + (ulFrontY[iLoop] * iZoom) - (45 * iZoom),
					"imgpillarfront", ascreen, iZoom, 0);
				break;
			case 0x32: /*** skeleton ***/
				ShowImage (imgskeleton,
					MapStartX() + (ulFrontX[iLoop] * iZoom) - (9 * iZoom),
					MapStartY() + (ulFrontY[iLoop] * iZoom) - (8 * iZoom),
					"imgskeleton", ascreen, iZoom, 0);
				break;
			case 0x33: /*** wall top left slash ***/
				ShowImage (imgwalltopleftslash,
					MapStartX() + (ulFrontX[iLoop] * iZoom) - (9 * iZoom),
					MapStartY() + (ulFrontY[iLoop] * iZoom) - (24 * iZoom),
					"imgwalltopleftslash", ascreen, iZoom, 0);
				break;
			case 0x34: /*** wall top left dot ***/
				ShowImage (imgwalltopleftdot,
					MapStartX() + (ulFrontX[iLoop] * iZoom) - (9 * iZoom),
					MapStartY() + (ulFrontY[iLoop] * iZoom) - (24 * iZoom),
					"imgwalltopleftdot", ascreen, iZoom, 0);
				break;
			case 0x35: /*** wall bottom left ***/
				ShowImage (imgwallbottomleft,
					MapStartX() + (ulFrontX[iLoop] * iZoom) - (9 * iZoom),
					MapStartY() + (ulFrontY[iLoop] * iZoom) - (24 * iZoom),
					"imgwallbottomleft", ascreen, iZoom, 0);
				break;
			case 0x36: /*** floor climbable ***/
				ShowImage (imgfloorclimbable,
					MapStartX() + (ulFrontX[iLoop] * iZoom) - (3 * iZoom),
					MapStartY() + (ulFrontY[iLoop] * iZoom) - (12 * iZoom),
					"imgfloorclimbable", ascreen, iZoom, 0);
				break;
			default:
				printf ("[ WARN ] Unknown front: %lu (0x%02X)\n",
					ulFrontTypeNr[ulFrontType[iLoop] + 1],
					(int)ulFrontTypeNr[ulFrontType[iLoop] + 1]);
				break;
		}
	}
}
/*****************************************************************************/
void ShowScreen (void)
/*****************************************************************************/
{
	int iX, iY;
	int iXFull, iYFull;

	/*** Used for looping. ***/
	int iRowLoop, iColumnLoop;

	/*** black ***/
	ShowImage (imgblack, 0, 0, "imgblack", ascreen, iScale, 1);

	/*** back ***/
	iY = 0;
	for (iRowLoop = 1; iRowLoop <= (int)ulHeight; iRowLoop++)
	{
		iX = 0;
		for (iColumnLoop = 1; iColumnLoop <= (int)ulWidth; iColumnLoop++)
		{
			iXFull = MapStartX() + (iX * iZoom);
			iYFull = MapStartY() + (iY * iZoom);
			ShowImage (imgback[ulBack[iRowLoop][iColumnLoop]],
				iXFull,
				iYFull,
				"imgback[]", ascreen, iZoom, 0);
			if ((iHoverRow == iRowLoop) && (iHoverColumn == iColumnLoop))
			{
				ShowImage (imghoverbacks, iXFull, iYFull,
					"imghoverbacks", ascreen, iZoom, 0);
			}
			iX+=16;
		}
		iY+=24;
	}

	if (iHideFront == 0) { ShowFrontTiles(); }

	/*** delete ***/
	if ((iDelX != -1) && (iDelY != -1))
	{
		ShowImage (imgdelete, MapStartX() + ((iDelX - 3) * iZoom),
			MapStartY() + ((iDelY - 3) * iZoom),
			"imgdelete", ascreen, iZoom, 0);
	}

	/*** interface ***/
	ShowImage (imginterface, 0, 0, "imginterface", ascreen, iScale, 1);
	if (iHideFront == 1)
		{ ShowImage (imgchkb, 108, 7, "imgchkb", ascreen, iScale, 1); }

	/*** prev level ***/
	if (iCurLevel != 0)
	{
		/*** on ***/
		if (iDownAt == 1)
		{
			ShowImage (imgprevon_1, 2, 2, "imgprevon_1",
				ascreen, iScale, 1); /*** down ***/
		} else {
			ShowImage (imgprevon_0, 2, 2, "imgprevon_0",
				ascreen, iScale, 1); /*** up ***/
		}
	} else {
		/*** off ***/
		ShowImage (imgprevoff, 2, 2, "imgprevoff", ascreen, iScale, 1);
	}

	/*** level number ***/
	snprintf (arText[0], MAX_TEXT, "%i", iCurLevel);
	DisplayText (33, 3, 20, 1, color_wh, 1);

	/*** next level ***/
	if (iCurLevel != MAX_LEVEL)
	{
		/*** on ***/
		if (iDownAt == 2)
		{
			ShowImage (imgnexton_1, 52, 2, "imgnexton_1",
				ascreen, iScale, 1); /*** down ***/
		} else {
			ShowImage (imgnexton_0, 52, 2, "imgnexton_0",
				ascreen, iScale, 1); /*** up ***/
		}
	} else {
		/*** off ***/
		ShowImage (imgnextoff, 52, 2, "imgnextoff", ascreen, iScale, 1);
	}

	/*** exe ***/
	if (iDownAt == 3)
	{
		ShowImage (imgexeon_1, 610, 2, "imgexeon_1",
			ascreen, iScale, 1); /*** down ***/
	} else {
		ShowImage (imgexeon_0, 610, 2, "imgexeon_0",
			ascreen, iScale, 1); /*** up ***/
	}

	/*** text ***/
	if (iDownAt == 4)
	{
		ShowImage (imgtexton_1, 635, 2, "imgtexton_1",
			ascreen, iScale, 1); /*** down ***/
	} else {
		ShowImage (imgtexton_0, 635, 2, "imgtexton_0",
			ascreen, iScale, 1); /*** up ***/
	}

	/*** playtest ***/
	if (iDownAt == 5)
	{
		ShowImage (imgplayon_1, 660, 2, "imgplayon_1",
			ascreen, iScale, 1); /*** down ***/
	} else {
		ShowImage (imgplayon_0, 660, 2, "imgplayon_0",
			ascreen, iScale, 1); /*** up ***/
	}

	/*** help ***/
	if (iDownAt == 6)
	{
		ShowImage (imghelpon_1, 685, 2, "imghelpon_1",
			ascreen, iScale, 1); /*** down ***/
	} else {
		ShowImage (imghelpon_0, 685, 2, "imghelpon_0",
			ascreen, iScale, 1); /*** up ***/
	}

	/*** save ***/
	if (iChanged != 0)
	{
		/*** on ***/
		if (iDownAt == 7)
		{
			ShowImage (imgsaveon_1, 2, 655, "imgsaveon_1",
				ascreen, iScale, 1); /*** down ***/
		} else {
			ShowImage (imgsaveon_0, 2, 655, "imgsaveon_0",
				ascreen, iScale, 1); /*** up ***/
		}
	} else {
		/*** off ***/
		ShowImage (imgsaveoff, 2, 655, "imgsaveoff", ascreen, iScale, 1);
	}

	/*** quit ***/
	if (iDownAt == 8)
	{
		ShowImage (imgquit_1, 685, 655, "imgquit_1",
			ascreen, iScale, 1); /*** down ***/
	} else {
		ShowImage (imgquit_0, 685, 655, "imgquit_0",
			ascreen, iScale, 1); /*** up ***/
	}

	/*** coordinates ***/
	if (InArea (MAP_LEFT, MAP_TOP, MAP_LEFT + MAP_WIDTH,
		MAP_TOP + MAP_HEIGHT) == 1) /*** map ***/
	{
		snprintf (arText[0], MAX_TEXT, "%.1f, %.1f",
			(float)(iXPos - MapStartX()) / iZoom,
			(float)(iYPos - MapStartY()) / iZoom);
		DisplayText (35, 660, 11, 1, color_wh, 1);
	}

	/*** Emulator information. ***/
	if (iEmulator == 1)
		{ ShowImage (imgemulator, 4, 29, "imgemulator", ascreen, iScale, 1); }

	/*** refresh screen ***/
	SDL_RenderPresent (ascreen);
}
/*****************************************************************************/
void Quit (void)
/*****************************************************************************/
{
	/*** Used for looping. ***/
	int iLoopFont;

	if (iChanged != 0) { PopUpSave(); }
	for (iLoopFont = 1; iLoopFont <= MAX_FONT_SIZE; iLoopFont++)
		{ TTF_CloseFont (font[iLoopFont]); }
	TTF_Quit();
	SDL_Quit();
	exit (EXIT_NORMAL);
}
/*****************************************************************************/
void LoadFonts (void)
/*****************************************************************************/
{
	/*** Used for looping. ***/
	int iLoopFont;

	for (iLoopFont = 1; iLoopFont <= MAX_FONT_SIZE; iLoopFont++)
	{
		font[iLoopFont] = TTF_OpenFont ("ttf/Bitstream-Vera-Sans-Bold.ttf",
			iLoopFont * iScale);
		if (font[iLoopFont] == NULL)
		{
			snprintf (sError, MAX_ERROR, "%s", "Could not load font!");
			ErrorAndExit();
		}
	}
}
/*****************************************************************************/
void MixAudio (void *unused, Uint8 *stream, int iLen)
/*****************************************************************************/
{
	int iTemp;
	int iAmount;

	if (unused != NULL) { } /*** To prevent warnings. ***/

	SDL_memset (stream, 0, iLen); /*** SDL2 ***/
	for (iTemp = 0; iTemp < NUM_SOUNDS; iTemp++)
	{
		iAmount = (sounds[iTemp].dlen-sounds[iTemp].dpos);
		if (iAmount > iLen)
		{
			iAmount = iLen;
		}
		SDL_MixAudio (stream, &sounds[iTemp].data[sounds[iTemp].dpos], iAmount,
			SDL_MIX_MAXVOLUME);
		sounds[iTemp].dpos += iAmount;
	}
}
/*****************************************************************************/
void PlaySound (char *sFile)
/*****************************************************************************/
{
	int iIndex;
	SDL_AudioSpec wave;
	Uint8 *data;
	Uint32 dlen;
	SDL_AudioCVT cvt;

	if (iNoAudio == 1) { return; }
	for (iIndex = 0; iIndex < NUM_SOUNDS; iIndex++)
	{
		if (sounds[iIndex].dpos == sounds[iIndex].dlen)
		{
			break;
		}
	}
	if (iIndex == NUM_SOUNDS) { return; }

	if (SDL_LoadWAV (sFile, &wave, &data, &dlen) == NULL)
	{
		printf ("[FAILED] Could not load %s: %s!\n", sFile, SDL_GetError());
		exit (EXIT_ERROR);
	}
	SDL_BuildAudioCVT (&cvt, wave.format, wave.channels, wave.freq, AUDIO_S16, 2,
		44100);
	/*** The "+ 1" is a workaround for SDL bug #2274. ***/
	cvt.buf = (Uint8 *)malloc (dlen * (cvt.len_mult + 1));
	memcpy (cvt.buf, data, dlen);
	cvt.len = dlen;
	SDL_ConvertAudio (&cvt);
	SDL_FreeWAV (data);

	if (sounds[iIndex].data)
	{
		free (sounds[iIndex].data);
	}
	SDL_LockAudio();
	sounds[iIndex].data = cvt.buf;
	sounds[iIndex].dlen = cvt.len_cvt;
	sounds[iIndex].dpos = 0;
	SDL_UnlockAudio();
}
/*****************************************************************************/
void PreLoad (char *sPath, char *sPNG, SDL_Texture **imgImage)
/*****************************************************************************/
{
	char sImage[MAX_IMG + 2];
	int iBarHeight;

	snprintf (sImage, MAX_IMG, "png%s%s%s%s", SLASH, sPath, SLASH, sPNG);
	*imgImage = IMG_LoadTexture (ascreen, sImage);
	if (!*imgImage)
	{
		printf ("[FAILED] IMG_LoadTexture: %s!\n", IMG_GetError());
		exit (EXIT_ERROR);
	}

	iPreLoaded++;
	iBarHeight = (int)(((float)iPreLoaded/(float)iNrToPreLoad) * BAR_FULL);
	if (iBarHeight >= iCurrentBarHeight + 10) { LoadingBar (iBarHeight); }
}
/*****************************************************************************/
void ShowImage (SDL_Texture *img, int iX, int iY, char *sImageInfo,
	SDL_Renderer *screen, float fMultiply, int iXYScale)
/*****************************************************************************/
{
	/* Usually, fMultiply is either iScale or iZoom.
	 *
	 * Also, usually, iXYScale is 1 with iScale and 0 with iZoom.
	 *
	 * If you use this function, make sure to verify the image is properly
	 * (re)scaled and (re)positioned when the main window is zoomed ("z")!
	 */

	SDL_Rect dest;
	SDL_Rect loc;
	int iWidth, iHeight;

	SDL_QueryTexture (img, NULL, NULL, &iWidth, &iHeight);
	loc.x = 0; loc.y = 0; loc.w = iWidth; loc.h = iHeight;
	if (iXYScale == 0)
	{
		dest.x = iX;
		dest.y = iY;
	} else {
		dest.x = iX * fMultiply;
		dest.y = iY * fMultiply;
	}
	dest.w = iWidth * fMultiply;
	dest.h = iHeight * fMultiply;

	/*** This is for the game animation. ***/
	if (iNoAnim == 0)
	{
		newticks = SDL_GetTicks();
		if (newticks > oldticksf + FPS_TORCH)
		{
			iFlameFrame++;
			if (iFlameFrame >= 6) { iFlameFrame = 1; }
			oldticksf = newticks;
		}
	}
	if (strcmp (sImageInfo, "imgtorchsprite") == 0)
	{
		loc.x = (iFlameFrame - 1) * 34;
		loc.w = loc.w / 5;
		dest.w = dest.w / 5;
	}
	if (strcmp (sImageInfo, "imgstatusbarsprite") == 0)
	{
		loc.x = (iStatusBarFrame - 1) * 20;
		loc.w = loc.w / 18;
		dest.w = dest.w / 18;
	}

	if (SDL_RenderCopy (screen, img, &loc, &dest) != 0)
	{
		printf ("[ WARN ] SDL_RenderCopy (%s): %s\n", sImageInfo, SDL_GetError());
	}
}
/*****************************************************************************/
void LoadingBar (int iBarHeight)
/*****************************************************************************/
{
	SDL_Rect bar;

	bar.x = (10 + 2) * iScale;
	bar.y = (662 + 10 - 2 - iBarHeight) * iScale;
	bar.w = (20 - 2 - 2) * iScale;
	bar.h = iBarHeight * iScale;
	SDL_SetRenderDrawColor (ascreen, 0x44, 0x44, 0x44, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect (ascreen, &bar);
	iCurrentBarHeight = iBarHeight;

	/*** refresh screen ***/
	SDL_RenderPresent (ascreen);
}
/*****************************************************************************/
void PrevLevel (void)
/*****************************************************************************/
{
	int iToLoad;

	iToLoad = iCurLevel - 1;

	if (iChanged != 0) { PopUpSave(); }

	if (iToLoad >= 0)
	{
		iCurLevel = iToLoad;
		LevelLoad (iToLoad);
		iChanged = 0;
		iDelX = -1;
		iDelY = -1;
		PlaySound ("wav/level_change.wav");
	}
}
/*****************************************************************************/
void NextLevel (void)
/*****************************************************************************/
{
	int iToLoad;

	iToLoad = iCurLevel + 1;

	if (iChanged != 0) { PopUpSave(); }

	if (iToLoad <= MAX_LEVEL)
	{
		iCurLevel = iToLoad;
		LevelLoad (iToLoad);
		iChanged = 0;
		iDelX = -1;
		iDelY = -1;
		PlaySound ("wav/level_change.wav");
	}
}
/*****************************************************************************/
void SetZoom (int iZoomTo)
/*****************************************************************************/
{
	if (iZoom != iZoomTo)
	{
		iZoom = iZoomTo;
		PlaySound ("wav/screen2or3.wav");
	}
}
/*****************************************************************************/
int InArea (int iUpperLeftX, int iUpperLeftY,
	int iLowerRightX, int iLowerRightY)
/*****************************************************************************/
{
	if ((iUpperLeftX * iScale <= iXPos) &&
		(iLowerRightX * iScale >= iXPos) &&
		(iUpperLeftY * iScale <= iYPos) &&
		(iLowerRightY * iScale >= iYPos))
	{
		return (1);
	} else {
		return (0);
	}
}
/*****************************************************************************/
int MapStartX (void)
/*****************************************************************************/
{
	float fReturn;
	float fLowest;

	fReturn = MAP_LEFT - ((float)ulPrinceX * iZoom) + (MAP_WIDTH / 2);
	fReturn += (float)iXPosDragOffset;
	if (fReturn > MAP_LEFT) { fReturn = MAP_LEFT; }
	fLowest = 0 - (((int)ulWidth * (16 * iZoom)) - MAP_WIDTH - MAP_LEFT);
	if (fReturn < fLowest) { fReturn = fLowest; }

	return (round (fReturn));
}
/*****************************************************************************/
int MapStartY (void)
/*****************************************************************************/
{
	float fReturn;
	float fLowest;

	fReturn = MAP_TOP - (((float)ulPrinceY - 2) * iZoom) + (MAP_HEIGHT / 2);
	fReturn += (float)iYPosDragOffset;
	if (fReturn > MAP_TOP) { fReturn = MAP_TOP; }
	fLowest = 0 - (((int)ulHeight * (24 * iZoom)) - MAP_HEIGHT - MAP_TOP);
	if (fReturn < fLowest) { fReturn = fLowest; }

	return (round (fReturn));
}
/*****************************************************************************/
void DisplayText (int iStartX, int iStartY, int iFontSize,
	int iLines, SDL_Color back, int iXYScale)
/*****************************************************************************/
{
	int iTemp;

	for (iTemp = 0; iTemp <= (iLines - 1); iTemp++)
	{
		if (strcmp (arText[iTemp], "") != 0)
		{
			message = TTF_RenderText_Shaded (font[iFontSize],
				arText[iTemp], color_bl, back);
			messaget = SDL_CreateTextureFromSurface (ascreen, message);
			offset.x = iStartX;
			offset.y = iStartY + (iTemp * (iFontSize + 4));
			offset.w = message->w; offset.h = message->h;
			CustomRenderCopy (messaget, NULL, &offset, "message", iXYScale);
			SDL_DestroyTexture (messaget); SDL_FreeSurface (message);
		}
	}
}
/*****************************************************************************/
void CustomRenderCopy (SDL_Texture* src, SDL_Rect* srcrect,
	SDL_Rect *dstrect, char *sImageInfo, int iXYScale)
/*****************************************************************************/
{
	SDL_Rect stuff;

	if (iXYScale == 0)
	{
		stuff.x = dstrect->x;
		stuff.y = dstrect->y;
	} else {
		stuff.x = dstrect->x * iScale;
		stuff.y = dstrect->y * iScale;
	}
	if (srcrect != NULL) /*** image ***/
	{
		stuff.w = dstrect->w * iScale;
		stuff.h = dstrect->h * iScale;
	} else { /*** font ***/
		stuff.w = dstrect->w;
		stuff.h = dstrect->h;
	}
	if (SDL_RenderCopy (ascreen, src, srcrect, &stuff) != 0)
	{
		printf ("[ WARN ] SDL_RenderCopy (%s): %s!\n",
			sImageInfo, SDL_GetError());
	}
}
/*****************************************************************************/
void Help (void)
/*****************************************************************************/
{
	int iHelp;
	SDL_Event event;

	iHelp = 1;

	PlaySound ("wav/popup.wav");
	ShowHelp();
	while (iHelp == 1)
	{
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_CONTROLLERBUTTONDOWN:
					/*** Nothing for now. ***/
					break;
				case SDL_CONTROLLERBUTTONUP:
					switch (event.cbutton.button)
					{
						case SDL_CONTROLLER_BUTTON_A:
							iHelp = 0; break;
					}
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
						case SDLK_KP_ENTER:
						case SDLK_RETURN:
						case SDLK_SPACE:
						case SDLK_o:
							iHelp = 0; break;
					}
					break;
				case SDL_MOUSEMOTION:
					iXPos = event.motion.x;
					iYPos = event.motion.y;
					if (InArea (92, 572, 92 + 528, 572 + 19) == 1)
					{
						SDL_SetCursor (curHand);
					} else {
						SDL_SetCursor (curArrow);
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 1)
					{
						if (InArea (610, 633, 610 + 85, 633 + 32) == 1) /*** OK ***/
							{ iHelpOK = 1; }
					}
					ShowHelp();
					break;
				case SDL_MOUSEBUTTONUP:
					iHelpOK = 0;
					if (event.button.button == 1)
					{
						if (InArea (610, 633, 610 + 85, 633 + 32) == 1) /*** OK ***/
							{ iHelp = 0; }
						if (InArea (92, 572, 92 + 528, 572 + 19) == 1)
							{ OpenURL ("https://github.com/EndeavourAccuracy/pophale"); }
					}
					ShowHelp();
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_EXPOSED)
						{ ShowHelp(); } break;
				case SDL_QUIT:
					Quit(); break;
			}
		}

		/*** prevent CPU eating ***/
		gamespeed = REFRESH_PROG;
		while ((SDL_GetTicks() - looptime) < gamespeed)
		{
			SDL_Delay (10);
		}
		looptime = SDL_GetTicks();
	}
	PlaySound ("wav/popup_close.wav");
	SDL_SetCursor (curArrow);
	ShowScreen();
}
/*****************************************************************************/
void ShowHelp (void)
/*****************************************************************************/
{
	/*** background ***/
	ShowImage (imghelp, 0, 0, "imghelp", ascreen, iScale, 1);

	/*** OK ***/
	switch (iHelpOK)
	{
		case 0: ShowImage (imgok[1], 610, 633, "imgok[1]",
			ascreen, iScale, 1); break; /*** up ***/
		case 1: ShowImage (imgok[2], 610, 633, "imgok[2]",
			ascreen, iScale, 1); break; /*** down ***/
	}

	/*** refresh screen ***/
	SDL_RenderPresent (ascreen);
}
/*****************************************************************************/
void OpenURL (char *sURL)
/*****************************************************************************/
{
#if defined WIN32 || _WIN32 || WIN64 || _WIN64
ShellExecute (NULL, "open", sURL, NULL, NULL, SW_SHOWNORMAL);
#else
pid_t pid;
pid = fork();
if (pid == 0)
{
	execl ("/usr/bin/xdg-open", "xdg-open", sURL, (char *)NULL);
	exit (EXIT_NORMAL);
}
#endif
}
/*****************************************************************************/
void ChangeBackAction (char *sAction)
/*****************************************************************************/
{
	if (strcmp (sAction, "select") == 0)
	{
		ulBack[iHoverRow][iHoverColumn] = TileNrToHex (iOnTile);
		iLastBack = TileNrToHex (iOnTile);
		iChanged++;
		iChangeBack = 0;
	}

	if (strcmp (sAction, "left") == 0)
	{
		do {
			iOnTile--;
			if ((iOnTile == 0) || (iOnTile == 10) || (iOnTile == 20) ||
				(iOnTile == 30) || (iOnTile == 40) || (iOnTile == 50))
				{ iOnTile+=10; }
		} while ((iOnTile == 21) || (iOnTile == 31) ||
			(iOnTile == 41) || (iOnTile == 51));
	}

	if (strcmp (sAction, "right") == 0)
	{
		do {
			iOnTile++;
			if ((iOnTile == 11) || (iOnTile == 21) || (iOnTile == 31) ||
				(iOnTile == 41) || (iOnTile == 51) || (iOnTile == 61))
				{ iOnTile-=10; }
		} while ((iOnTile == 21) || (iOnTile == 31) ||
			(iOnTile == 41) || (iOnTile == 51));
	}

	if (strcmp (sAction, "up") == 0)
	{
		do {
			iOnTile-=10;
			if (iOnTile <= 0) { iOnTile+=60; }
		} while ((iOnTile == 21) || (iOnTile == 31) ||
			(iOnTile == 41) || (iOnTile == 51));
	}

	if (strcmp (sAction, "down") == 0)
	{
		do {
			iOnTile+=10;
			if (iOnTile >= 61) { iOnTile-=60; }
		} while ((iOnTile == 21) || (iOnTile == 31) ||
			(iOnTile == 41) || (iOnTile == 51));
	}
}
/*****************************************************************************/
void ChangeBack (void)
/*****************************************************************************/
{
	SDL_Event event;
	int iOldXPos, iOldYPos;
	int iHoverTile;

	/*** Used for looping. ***/
	int iXLoop, iYLoop;

	iChangeBack = 1;
	iOnTile = TileHexToNr (ulBack[iHoverRow][iHoverColumn]);

	ShowChangeBack();
	while (iChangeBack == 1)
	{
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_CONTROLLERBUTTONDOWN:
					/*** Nothing for now. ***/
					break;
				case SDL_CONTROLLERBUTTONUP:
					switch (event.cbutton.button)
					{
						case SDL_CONTROLLER_BUTTON_A:
							ChangeBackAction ("select");
							break;
						case SDL_CONTROLLER_BUTTON_B:
							iChangeBack = 0; break;
						case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
							ChangeBackAction ("left"); break;
						case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
							ChangeBackAction ("right"); break;
						case SDL_CONTROLLER_BUTTON_DPAD_UP:
							ChangeBackAction ("up"); break;
						case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
							ChangeBackAction ("down"); break;
					}
					ShowChangeBack();
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_KP_ENTER:
						case SDLK_RETURN:
						case SDLK_SPACE:
							ChangeBackAction ("select");
							break;
						case SDLK_ESCAPE:
						case SDLK_c:
							iChangeBack = 0; break;
						case SDLK_LEFT:
							ChangeBackAction ("left");
							break;
						case SDLK_RIGHT:
							ChangeBackAction ("right");
							break;
						case SDLK_UP:
							ChangeBackAction ("up");
							break;
						case SDLK_DOWN:
							ChangeBackAction ("down");
							break;
					}
					ShowChangeBack();
					break;
				case SDL_MOUSEMOTION:
					iOldXPos = iXPos;
					iOldYPos = iYPos;
					iXPos = event.motion.x;
					iYPos = event.motion.y;
					if ((iOldXPos == iXPos) && (iOldYPos == iYPos)) { break; }

					for (iYLoop = 0; iYLoop <= 5; iYLoop++)
					{
						for (iXLoop = 0; iXLoop <= 9; iXLoop++)
						{
							if (InArea (4 + (iXLoop * (64 + 3)),
								4 + (iYLoop * (96 + 3)),
								4 + (iXLoop * (64 + 3)) + 64,
								4 + (iYLoop * (96 + 3)) + 96) == 1)
							{
								iHoverTile = (iYLoop * 10) + iXLoop + 1;
								if ((iHoverTile != 21) && (iHoverTile != 31) &&
									(iHoverTile != 41) && (iHoverTile != 51))
								{
									iOnTile = iHoverTile;
								}
							}
						}
					}
					ShowChangeBack();
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 1)
					{
						if (InArea (674, 2, 674 + 36, 2 + 678) == 1)
							{ iCloseOn = 1; } /*** close ***/
					}
					ShowChangeBack();
					break;
				case SDL_MOUSEBUTTONUP:
					iCloseOn = 0;

					if (event.button.button == 1)
					{
						if (InArea (674, 2, 674 + 36, 2 + 678) == 1)
							{ iChangeBack = 0; } /*** close ***/
						if (InArea (4, 4, 671, 595) == 1)
							{ ChangeBackAction ("select"); }
					}
					ShowChangeBack();
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_EXPOSED)
						{ ShowChangeBack(); } break;
				case SDL_QUIT:
					Quit(); break;
			}
		}

		/*** prevent CPU eating ***/
		gamespeed = REFRESH_PROG;
		while ((SDL_GetTicks() - looptime) < gamespeed)
		{
			SDL_Delay (10);
		}
		looptime = SDL_GetTicks();
	}
	PlaySound ("wav/ok_close.wav");
	ShowScreen();
}
/*****************************************************************************/
void ShowChangeBack (void)
/*****************************************************************************/
{
	int iLiveTile;
	int iLiveRow, iLiveColumn;
	int iLiveX, iLiveY;
	int iOnRow, iOnColumn;
	int iOnX, iOnY;

	ShowImage (imgbacktiles, 0, 0, "imgbacktiles", ascreen, iScale, 1);

	/*** close ***/
	switch (iCloseOn)
	{
		case 0: ShowImage (imgclose[1], 674, 2, "imgclose[1]",
			ascreen, iScale, 1); break; /*** up ***/
		case 1: ShowImage (imgclose[2], 674, 2, "imgclose[2]",
			ascreen, iScale, 1); break; /*** down ***/
	}

	/*** live tile ***/
	iLiveTile = TileHexToNr (ulBack[iHoverRow][iHoverColumn]);
	iLiveRow = ((iLiveTile - 1) / 10) + 1;
	iLiveColumn = iLiveTile % 10;
	if (iLiveColumn == 0) { iLiveColumn = 10; }
	iLiveX = 1 + ((iLiveColumn - 1) * (64 + 3));
	iLiveY = 1 + ((iLiveRow - 1) * (96 + 3));
	ShowImage (imghoverbackllive, iLiveX, iLiveY,
		"imghoverbackllive", ascreen, iScale, 1);

	/*** on tile ***/
	iOnRow = ((iOnTile - 1) / 10) + 1;
	iOnColumn = iOnTile % 10;
	if (iOnColumn == 0) { iOnColumn = 10; }
	iOnX = 1 + ((iOnColumn - 1) * (64 + 3));
	iOnY = 1 + ((iOnRow - 1) * (96 + 3));
	ShowImage (imghoverbackl, iOnX, iOnY,
		"imghoverbackl", ascreen, iScale, 1);

	/*** refresh screen ***/
	SDL_RenderPresent (ascreen);
}
/*****************************************************************************/
int TileHexToNr (int iHex)
/*****************************************************************************/
{
	int iReturn;

	switch (iHex)
	{
		case 0x00: iReturn = 37; break;
		case 0x01: iReturn = 4; break;
		case 0x02: iReturn = 10; break;
		case 0x03: iReturn = 7; break;
		case 0x04: iReturn = 20; break;
		case 0x05: iReturn = 5; break;
		case 0x06: iReturn = 45; break;
		case 0x07: iReturn = 26; break;
		case 0x08: iReturn = 25; break;
		case 0x09: iReturn = 27; break;
		case 0x0A: iReturn = 43; break;
		case 0x0B: iReturn = 30; break;
		case 0x0C: iReturn = 3; break;
		case 0x0D: iReturn = 8; break;
		case 0x0E: iReturn = 56; break;
		case 0x0F: iReturn = 58; break;
		case 0x10: iReturn = 59; break;
		case 0x11: iReturn = 17; break;
		case 0x12: iReturn = 19; break;
		case 0x13: iReturn = 15; break;
		case 0x14: iReturn = 18; break;
		case 0x15: iReturn = 49; break;
		case 0x16: iReturn = 47; break;
		case 0x17: iReturn = 55; break;
		case 0x18: iReturn = 44; break;
		case 0x19: iReturn = 40; break;
		case 0x1A: iReturn = 13; break;
		case 0x1B: iReturn = 9; break;
		case 0x1C: iReturn = 33; break;
		case 0x1D: iReturn = 22; break;
		case 0x1E: iReturn = 23; break;
		case 0x1F: iReturn = 24; break;
		case 0x20: iReturn = 35; break;
		case 0x21: iReturn = 1; break;
		case 0x22: iReturn = 2; break;
		case 0x23: iReturn = 36; break;
		case 0x24: iReturn = 32; break;
		case 0x25: iReturn = 46; break;
		case 0x26: iReturn = 42; break;
		case 0x27: iReturn = 50; break;
		case 0x28: iReturn = 16; break;
		case 0x29: iReturn = 57; break;
		case 0x2A: iReturn = 48; break;
		case 0x2B: iReturn = 39; break;
		case 0x2C: iReturn = 53; break;
		case 0x2D: iReturn = 54; break;
		case 0x2E: iReturn = 29; break;
		case 0x2F: iReturn = 11; break;
		case 0x30: iReturn = 12; break;
		case 0x31: iReturn = 14; break;
		case 0x32: iReturn = 34; break;
		case 0x33: iReturn = 28; break;
		case 0x34: iReturn = 52; break;
		case 0x35: iReturn = 60; break;
		case 0x36: iReturn = 38; break;
		case 0x37: iReturn = 6; break;
		default:
			printf ("[ WARN ] Invalid back value: %02X!\n", iHex);
			iReturn = 20; /*** Fallback. ***/
	}

	return (iReturn);
}
/*****************************************************************************/
int TileNrToHex (int iNr)
/*****************************************************************************/
{
	int iReturn;

	switch (iNr)
	{
		case 1: iReturn = 0x21; break;
		case 2: iReturn = 0x22; break;
		case 3: iReturn = 0x0C; break;
		case 4: iReturn = 0x01; break;
		case 5: iReturn = 0x05; break;
		case 6: iReturn = 0x37; break;
		case 7: iReturn = 0x03; break;
		case 8: iReturn = 0x0D; break;
		case 9: iReturn = 0x1B; break;
		case 10: iReturn = 0x02; break;
		case 11: iReturn = 0x2F; break;
		case 12: iReturn = 0x30; break;
		case 13: iReturn = 0x1A; break;
		case 14: iReturn = 0x31; break;
		case 15: iReturn = 0x13; break;
		case 16: iReturn = 0x28; break;
		case 17: iReturn = 0x11; break;
		case 18: iReturn = 0x14; break;
		case 19: iReturn = 0x12; break;
		case 20: iReturn = 0x04; break;
		/*** no 21 ***/
		case 22: iReturn = 0x1D; break;
		case 23: iReturn = 0x1E; break;
		case 24: iReturn = 0x1F; break;
		case 25: iReturn = 0x08; break;
		case 26: iReturn = 0x07; break;
		case 27: iReturn = 0x09; break;
		case 28: iReturn = 0x33; break;
		case 29: iReturn = 0x2E; break;
		case 30: iReturn = 0x0B; break;
		/*** no 31 ***/
		case 32: iReturn = 0x24; break;
		case 33: iReturn = 0x1C; break;
		case 34: iReturn = 0x32; break;
		case 35: iReturn = 0x20; break;
		case 36: iReturn = 0x23; break;
		case 37: iReturn = 0x00; break;
		case 38: iReturn = 0x36; break;
		case 39: iReturn = 0x2B; break;
		case 40: iReturn = 0x19; break;
		/*** no 41 ***/
		case 42: iReturn = 0x26; break;
		case 43: iReturn = 0x0A; break;
		case 44: iReturn = 0x18; break;
		case 45: iReturn = 0x06; break;
		case 46: iReturn = 0x25; break;
		case 47: iReturn = 0x16; break;
		case 48: iReturn = 0x2A; break;
		case 49: iReturn = 0x15; break;
		case 50: iReturn = 0x27; break;
		/*** no 51 ***/
		case 52: iReturn = 0x34; break;
		case 53: iReturn = 0x2C; break;
		case 54: iReturn = 0x2D; break;
		case 55: iReturn = 0x17; break;
		case 56: iReturn = 0x0E; break;
		case 57: iReturn = 0x29; break;
		case 58: iReturn = 0x0F; break;
		case 59: iReturn = 0x10; break;
		case 60: iReturn = 0x35; break;
		default:
			printf ("[ WARN ] Invalid tile number: %i!\n", iNr);
			iReturn = 0x04; /*** Fallback. ***/
	}

	return (iReturn);
}
/*****************************************************************************/
void ChangeFrontAction (char *sAction)
/*****************************************************************************/
{
	if (strcmp (sAction, "select") == 0)
	{
		switch (iOnTile)
		{
			case 1: /*** torch ***/
				AddFront (iCFX, iCFY, 0x30);
				iChanged++;
				iChangeFront = 0;
				break;
			case 2: /*** pillar front ***/
				AddFront (AlignX (iCFX, 4), AlignY (iCFY, 21), 0x31);
				iChanged++;
				iChangeFront = 0;
				break;
			case 3: /*** skeleton ***/
				AddFront (iCFX, AlignY (iCFY, 13), 0x32);
				iChanged++;
				iChangeFront = 0;
				break;
			case 4: /*** wall top left slash ***/
				AddFront (AlignX (iCFX, 9), AlignY (iCFY, 0), 0x33);
				iChanged++;
				iChangeFront = 0;
				break;
			case 5: /*** wall top left dot ***/
				AddFront (AlignX (iCFX, 9), AlignY (iCFY, 0), 0x34);
				iChanged++;
				iChangeFront = 0;
				break;
			case 6: /*** wall bottom left ***/
				AddFront (AlignX (iCFX, 11), AlignY (iCFY, 22), 0x35);
				iChanged++;
				iChangeFront = 0;
				break;
			case 7: /*** floor climbable ***/
				AddFront (AlignX (iCFX, 3), AlignY (iCFY, 3), 0x36);
				iChanged++;
				iChangeFront = 0;
				break;
			case 8: /*** prince ***/
				ulPrinceX = iCFX;
				ulPrinceY = AlignY (iCFY, 19);
				iChanged++;
				iChangeFront = 0;
				break;
			case 9: /*** exit trigger ***/
				ulExitTriggerX = iCFX;
				ulExitTriggerY = AlignY (iCFY, 18);
				iChanged++;
				iChangeFront = 0;
				break;
			case 10: /*** save trigger ***/
				ulSaveTriggerX = AlignX (iCFX, 7);
				ulSaveTriggerY = AlignY (iCFY, 19);
				iChanged++;
				iChangeFront = 0;
				break;
			case 11: /*** entrance image ***/
				ulEntranceImageX = iCFX;
				ulEntranceImageY = AlignY (iCFY, 9);
				iChanged++;
				iChangeFront = 0;
				break;
			case 12: /*** exit image ***/
				ulExitImageX = iCFX;
				ulExitImageY = AlignY (iCFY, 9);
				iChanged++;
				iChangeFront = 0;
				break;
			case 13: /*** chomper ***/
				ulNrChompers++;
				ulChomperX[ulNrChompers] = iCFX;
				ulChomperY[ulNrChompers] = AlignY (iCFY, 22);
				ulChomperA[ulNrChompers] = iFront1301;
				iChanged++;
				iChangeFront = 0;
				break;
			case 14: /*** spikes ***/
				ulNrSpikes++;
				ulSpikeX[ulNrSpikes] = AlignX (iCFX, 9);
				ulSpikeY[ulNrSpikes] = AlignY (iCFY, 0);
				ulSpikeRight[ulNrSpikes] = iFront1401;
				iChanged++;
				iChangeFront = 0;
				break;
			case 15: /*** gate ***/
				ulNrGates++;
				/* The AlignX() here is for the gate to line up with the
				 * corresponding back tile.
				 */
				ulGateX[ulNrGates] = AlignX (iCFX, 6);
				ulGateY[ulNrGates] = AlignY (iCFY, 22);
				ulGateTimeOpen[ulNrGates] = (iFront1501 * 12);
				iChanged++;
				iChangeFront = 0;
				break;
			case 16: /*** raise ***/
				ulNrRaise++;
				ulRaiseGate[ulNrRaise] = iFront1601;
				ulRaiseX[ulNrRaise] = AlignX (iCFX, 15);
				ulRaiseY[ulNrRaise] = AlignY (iCFY, 0);
				iChanged++;
				iChangeFront = 0;
				break;
			case 17: /*** guard ***/
				ulNrGuards++;
				ulGuardX[ulNrGuards] = iCFX;
				ulGuardY[ulNrGuards] = AlignY (iCFY, 18);
				ulGuardDir[ulNrGuards] = iFront1701;
				ulGuardHP[ulNrGuards] = iFront1702;
				ulGuardA[ulNrGuards] = iFront1703;
				ulGuardB[ulNrGuards] = iFront1704;
				ulGuardC[ulNrGuards] = iFront1705;
				ulGuardD[ulNrGuards] = iFront1706;
				ulGuardE[ulNrGuards] = iFront1707;
				ulGuardF[ulNrGuards] = iFront1708;
				iChanged++;
				iChangeFront = 0;
				break;
			case 18: /*** potion ***/
				ulNrPotions++;
				ulPotionType[ulNrPotions] = iFront1801;
				switch (iFront1801)
				{
					case 0:
						ulPotionX[ulNrPotions] = AlignX (iCFX, 13);
						ulPotionY[ulNrPotions] = AlignY (iCFY, 18);
						break;
					case 1:
						ulPotionX[ulNrPotions] = AlignX (iCFX, 12);
						ulPotionY[ulNrPotions] = AlignY (iCFY, 18);
						break;
					case 2:
						ulPotionX[ulNrPotions] = AlignX (iCFX, 12);
						ulPotionY[ulNrPotions] = AlignY (iCFY, 18);
						break;
					case 3:
						ulPotionX[ulNrPotions] = AlignX (iCFX, 8);
						ulPotionY[ulNrPotions] = AlignY (iCFY, 22);
						break;
				}
				iChanged++;
				iChangeFront = 0;
				break;
			case 19: /*** loose ***/
				ulNrLoose++;
				switch (iFront1901)
				{
					case 0:
						ulLooseX[ulNrLoose] = AlignX (iCFX, 12);
						break;
					case 1:
						ulLooseX[ulNrLoose] = AlignX (iCFX, 14);
						break;
				}
				ulLooseY[ulNrLoose] = AlignY (iCFY, 1);
				ulLooseRight[ulNrLoose] = iFront1901;
				iChanged++;
				iChangeFront = 0;
				break;
		}
	}

	if (strcmp (sAction, "left") == 0)
	{
		switch (iOnTile)
		{
			case 1: iOnTile = 15; break;
			case 2: iOnTile = 16; break;
			case 3: iOnTile = 17; break;
			case 4: iOnTile = 17; break;
			case 5: iOnTile = 17; break;
			case 6: iOnTile = 18; break;
			case 7: iOnTile = 19; break;
			case 8: iOnTile = 1; break;
			case 9: iOnTile = 2; break;
			case 10: iOnTile = 3; break;
			case 11: iOnTile = 4; break;
			case 12: iOnTile = 5; break;
			case 13: iOnTile = 6; break;
			case 14: iOnTile = 7; break;
			case 15: iOnTile = 8; break;
			case 16: iOnTile = 9; break;
			case 17: iOnTile = 10; break;
			case 18: iOnTile = 13; break;
			case 19: iOnTile = 14; break;
		}
	}

	if (strcmp (sAction, "right") == 0)
	{
		switch (iOnTile)
		{
			case 1: iOnTile = 8; break;
			case 2: iOnTile = 9; break;
			case 3: iOnTile = 10; break;
			case 4: iOnTile = 11; break;
			case 5: iOnTile = 12; break;
			case 6: iOnTile = 13; break;
			case 7: iOnTile = 14; break;
			case 8: iOnTile = 15; break;
			case 9: iOnTile = 16; break;
			case 10: iOnTile = 17; break;
			case 11: iOnTile = 17; break;
			case 12: iOnTile = 17; break;
			case 13: iOnTile = 18; break;
			case 14: iOnTile = 19; break;
			case 15: iOnTile = 1; break;
			case 16: iOnTile = 2; break;
			case 17: iOnTile = 3; break;
			case 18: iOnTile = 6; break;
			case 19: iOnTile = 7; break;
		}
	}

	if (strcmp (sAction, "up") == 0)
	{
		switch (iOnTile)
		{
			case 1: iOnTile = 7; break;
			case 2: iOnTile = 1; break;
			case 3: iOnTile = 2; break;
			case 4: iOnTile = 3; break;
			case 5: iOnTile = 4; break;
			case 6: iOnTile = 5; break;
			case 7: iOnTile = 6; break;
			case 8: iOnTile = 14; break;
			case 9: iOnTile = 8; break;
			case 10: iOnTile = 9; break;
			case 11: iOnTile = 10; break;
			case 12: iOnTile = 11; break;
			case 13: iOnTile = 12; break;
			case 14: iOnTile = 13; break;
			case 15: iOnTile = 19; break;
			case 16: iOnTile = 15; break;
			case 17: iOnTile = 16; break;
			case 18: iOnTile = 17; break;
			case 19: iOnTile = 18; break;
		}
	}

	if (strcmp (sAction, "down") == 0)
	{
		switch (iOnTile)
		{
			case 1: iOnTile = 2; break;
			case 2: iOnTile = 3; break;
			case 3: iOnTile = 4; break;
			case 4: iOnTile = 5; break;
			case 5: iOnTile = 6; break;
			case 6: iOnTile = 7; break;
			case 7: iOnTile = 1; break;
			case 8: iOnTile = 9; break;
			case 9: iOnTile = 10; break;
			case 10: iOnTile = 11; break;
			case 11: iOnTile = 12; break;
			case 12: iOnTile = 13; break;
			case 13: iOnTile = 14; break;
			case 14: iOnTile = 8; break;
			case 15: iOnTile = 16; break;
			case 16: iOnTile = 17; break;
			case 17: iOnTile = 18; break;
			case 18: iOnTile = 19; break;
			case 19: iOnTile = 15; break;
		}
	}
}
/*****************************************************************************/
void ChangeFront (void)
/*****************************************************************************/
{
	SDL_Event event;
	int iOldXPos, iOldYPos;

	iChangeFront = 1;
	iOnTile = 1;
	iCFX = (iXPos - MapStartX()) / iZoom;
	iCFY = (iYPos - MapStartY()) / iZoom;

	iFront0101 = 0;
	iFront0102 = 0;
	iFront0201 = 0;
	iFront0202 = 0;
	iFront0301 = 0;
	iFront0302 = 0;
	iFront0401 = 0;
	iFront0402 = 0;
	iFront0501 = 0;
	iFront0502 = 0;
	iFront0601 = 0;
	iFront0602 = 0;
	iFront0701 = 0;
	iFront0702 = 0;
	iFront1301 = 0;
	iFront1401 = 0;
	iFront1501 = 0;
	iFront1601 = 0;
	iFront1701 = 0;
	iFront1702 = 3; /*** Hit points ***/
	iFront1703 = 0;
	iFront1704 = 5; /*** Imp. block ***/
	iFront1705 = 0;
	iFront1706 = 6;
	iFront1707 = 0;
	iFront1708 = 8; /*** Strike delay ***/
	iFront1801 = 0;
	iFront1901 = 0;

	ShowChangeFront();
	while (iChangeFront == 1)
	{
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_CONTROLLERBUTTONDOWN:
					/*** Nothing for now. ***/
					break;
				case SDL_CONTROLLERBUTTONUP:
					switch (event.cbutton.button)
					{
						case SDL_CONTROLLER_BUTTON_A:
							ChangeFrontAction ("select");
							break;
						case SDL_CONTROLLER_BUTTON_B:
							iChangeFront = 0; break;
						case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
							ChangeFrontAction ("left"); break;
						case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
							ChangeFrontAction ("right"); break;
						case SDL_CONTROLLER_BUTTON_DPAD_UP:
							ChangeFrontAction ("up"); break;
						case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
							ChangeFrontAction ("down"); break;
					}
					ShowChangeFront();
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_KP_ENTER:
						case SDLK_RETURN:
						case SDLK_SPACE:
							ChangeFrontAction ("select");
							break;
						case SDLK_ESCAPE:
						case SDLK_c:
							iChangeFront = 0; break;
						case SDLK_LEFT:
							ChangeFrontAction ("left");
							break;
						case SDLK_RIGHT:
							ChangeFrontAction ("right");
							break;
						case SDLK_UP:
							ChangeFrontAction ("up");
							break;
						case SDLK_DOWN:
							ChangeFrontAction ("down");
							break;
					}
					ShowChangeFront();
					break;
				case SDL_MOUSEMOTION:
					iOldXPos = iXPos;
					iOldYPos = iYPos;
					iXPos = event.motion.x;
					iYPos = event.motion.y;
					if ((iOldXPos == iXPos) && (iOldYPos == iYPos)) { break; }

					if (InArea (2, 2, 2 + 96, 2 + 96) == 1) { iOnTile = 1; }
					if (InArea (2, 99, 2 + 96, 99 + 96) == 1) { iOnTile = 2; }
					if (InArea (2, 196, 2 + 96, 196 + 96) == 1) { iOnTile = 3; }
					if (InArea (2, 293, 2 + 96, 293 + 96) == 1) { iOnTile = 4; }
					if (InArea (2, 390, 2 + 96, 390 + 96) == 1) { iOnTile = 5; }
					if (InArea (2, 487, 2 + 96, 487 + 96) == 1) { iOnTile = 6; }
					if (InArea (2, 584, 2 + 96, 584 + 96) == 1) { iOnTile = 7; }
					if (InArea (226, 2, 226 + 96, 2 + 96) == 1) { iOnTile = 8; }
					if (InArea (226, 99, 226 + 96, 99 + 96) == 1) { iOnTile = 9; }
					if (InArea (226, 196, 226 + 96, 196 + 96) == 1) { iOnTile = 10; }
					if (InArea (226, 293, 226 + 96, 293 + 96) == 1) { iOnTile = 11; }
					if (InArea (226, 390, 226 + 96, 390 + 96) == 1) { iOnTile = 12; }
					if (InArea (226, 487, 226 + 96, 487 + 96) == 1) { iOnTile = 13; }
					if (InArea (226, 584, 226 + 96, 584 + 96) == 1) { iOnTile = 14; }
					if (InArea (450, 2, 450 + 96, 2 + 96) == 1) { iOnTile = 15; }
					if (InArea (450, 99, 450 + 96, 99 + 96) == 1) { iOnTile = 16; }
					if (InArea (450, 196, 450 + 96, 196 + 96) == 1) { iOnTile = 17; }
					if (InArea (450, 487, 450 + 96, 487 + 96) == 1) { iOnTile = 18; }
					if (InArea (450, 584, 450 + 96, 584 + 96) == 1) { iOnTile = 19; }

					ShowChangeFront();
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 1)
					{
						if (InArea (674, 2, 674 + 36, 2 + 678) == 1)
							{ iCloseOn = 1; } /*** close ***/
					}
					ShowChangeFront();
					break;
				case SDL_MOUSEBUTTONUP:
					iCloseOn = 0;

					if (event.button.button == 1)
					{
						if (InArea (674, 2, 674 + 36, 2 + 678) == 1)
							{ iChangeFront = 0; } /*** close ***/

						/*** torch ***/
						PlusMinus (&iFront0101, 105, 30, 0, 255, -10, 0);
						PlusMinus (&iFront0101, 120, 30, 0, 255, -1, 0);
						PlusMinus (&iFront0101, 190, 30, 0, 255, +1, 0);
						PlusMinus (&iFront0101, 205, 30, 0, 255, +10, 0);
						PlusMinus (&iFront0102, 105, 75, 0, 255, -10, 0);
						PlusMinus (&iFront0102, 120, 75, 0, 255, -1, 0);
						PlusMinus (&iFront0102, 190, 75, 0, 255, +1, 0);
						PlusMinus (&iFront0102, 205, 75, 0, 255, +10, 0);

						/*** pillar front ***/
						PlusMinus (&iFront0201, 105, 127, 0, 255, -10, 0);
						PlusMinus (&iFront0201, 120, 127, 0, 255, -1, 0);
						PlusMinus (&iFront0201, 190, 127, 0, 255, +1, 0);
						PlusMinus (&iFront0201, 205, 127, 0, 255, +10, 0);
						PlusMinus (&iFront0202, 105, 172, 0, 255, -10, 0);
						PlusMinus (&iFront0202, 120, 172, 0, 255, -1, 0);
						PlusMinus (&iFront0202, 190, 172, 0, 255, +1, 0);
						PlusMinus (&iFront0202, 205, 172, 0, 255, +10, 0);

						/*** skeleton ***/
						PlusMinus (&iFront0301, 105, 224, 0, 255, -10, 0);
						PlusMinus (&iFront0301, 120, 224, 0, 255, -1, 0);
						PlusMinus (&iFront0301, 190, 224, 0, 255, +1, 0);
						PlusMinus (&iFront0301, 205, 224, 0, 255, +10, 0);
						PlusMinus (&iFront0302, 105, 269, 0, 255, -10, 0);
						PlusMinus (&iFront0302, 120, 269, 0, 255, -1, 0);
						PlusMinus (&iFront0302, 190, 269, 0, 255, +1, 0);
						PlusMinus (&iFront0302, 205, 269, 0, 255, +10, 0);

						/*** wall top left slash ***/
						PlusMinus (&iFront0401, 105, 321, 0, 255, -10, 0);
						PlusMinus (&iFront0401, 120, 321, 0, 255, -1, 0);
						PlusMinus (&iFront0401, 190, 321, 0, 255, +1, 0);
						PlusMinus (&iFront0401, 205, 321, 0, 255, +10, 0);
						PlusMinus (&iFront0402, 105, 366, 0, 255, -10, 0);
						PlusMinus (&iFront0402, 120, 366, 0, 255, -1, 0);
						PlusMinus (&iFront0402, 190, 366, 0, 255, +1, 0);
						PlusMinus (&iFront0402, 205, 366, 0, 255, +10, 0);

						/*** wall top left dot ***/
						PlusMinus (&iFront0501, 105, 418, 0, 255, -10, 0);
						PlusMinus (&iFront0501, 120, 418, 0, 255, -1, 0);
						PlusMinus (&iFront0501, 190, 418, 0, 255, +1, 0);
						PlusMinus (&iFront0501, 205, 418, 0, 255, +10, 0);
						PlusMinus (&iFront0502, 105, 463, 0, 255, -10, 0);
						PlusMinus (&iFront0502, 120, 463, 0, 255, -1, 0);
						PlusMinus (&iFront0502, 190, 463, 0, 255, +1, 0);
						PlusMinus (&iFront0502, 205, 463, 0, 255, +10, 0);

						/*** floor climbable ***/
						PlusMinus (&iFront0601, 105, 515, 0, 255, -10, 0);
						PlusMinus (&iFront0601, 120, 515, 0, 255, -1, 0);
						PlusMinus (&iFront0601, 190, 515, 0, 255, +1, 0);
						PlusMinus (&iFront0601, 205, 515, 0, 255, +10, 0);
						PlusMinus (&iFront0602, 105, 560, 0, 255, -10, 0);
						PlusMinus (&iFront0602, 120, 560, 0, 255, -1, 0);
						PlusMinus (&iFront0602, 190, 560, 0, 255, +1, 0);
						PlusMinus (&iFront0602, 205, 560, 0, 255, +10, 0);

						/*** wall bottom left ***/
						PlusMinus (&iFront0701, 105, 612, 0, 255, -10, 0);
						PlusMinus (&iFront0701, 120, 612, 0, 255, -1, 0);
						PlusMinus (&iFront0701, 190, 612, 0, 255, +1, 0);
						PlusMinus (&iFront0701, 205, 612, 0, 255, +10, 0);
						PlusMinus (&iFront0702, 105, 657, 0, 255, -10, 0);
						PlusMinus (&iFront0702, 120, 657, 0, 255, -1, 0);
						PlusMinus (&iFront0702, 190, 657, 0, 255, +1, 0);
						PlusMinus (&iFront0702, 205, 657, 0, 255, +10, 0);

						/*** chomper ***/
						PlusMinus (&iFront1301, 329, 515, 0, 255, -10, 0);
						PlusMinus (&iFront1301, 344, 515, 0, 255, -1, 0);
						PlusMinus (&iFront1301, 414, 515, 0, 255, +1, 0);
						PlusMinus (&iFront1301, 429, 515, 0, 255, +10, 0);

						/*** spikes ***/
						PlusMinus (&iFront1401, 329, 612, 0, 1, -10, 0);
						PlusMinus (&iFront1401, 344, 612, 0, 1, -1, 0);
						PlusMinus (&iFront1401, 414, 612, 0, 1, +1, 0);
						PlusMinus (&iFront1401, 429, 612, 0, 1, +10, 0);

						/*** gate ***/
						PlusMinus (&iFront1501, 553, 30, 0, 255, -10, 0);
						PlusMinus (&iFront1501, 568, 30, 0, 255, -1, 0);
						PlusMinus (&iFront1501, 638, 30, 0, 255, +1, 0);
						PlusMinus (&iFront1501, 653, 30, 0, 255, +10, 0);

						/*** raise ***/
						PlusMinus (&iFront1601, 553, 127, 0, 255, -10, 0);
						PlusMinus (&iFront1601, 568, 127, 0, 255, -1, 0);
						PlusMinus (&iFront1601, 638, 127, 0, 255, +1, 0);
						PlusMinus (&iFront1601, 653, 127, 0, 255, +10, 0);

						/*** guard ***/
						PlusMinus (&iFront1701, 553, 224, 0, 1, -10, 0);
						PlusMinus (&iFront1701, 568, 224, 0, 1, -1, 0);
						PlusMinus (&iFront1701, 638, 224, 0, 1, +1, 0);
						PlusMinus (&iFront1701, 653, 224, 0, 1, +10, 0);
						PlusMinus (&iFront1702, 553, 269, 0, 255, -10, 0);
						PlusMinus (&iFront1702, 568, 269, 0, 255, -1, 0);
						PlusMinus (&iFront1702, 638, 269, 0, 255, +1, 0);
						PlusMinus (&iFront1702, 653, 269, 0, 255, +10, 0);
						PlusMinus (&iFront1703, 553, 318, 0, 255, -10, 0);
						PlusMinus (&iFront1703, 568, 318, 0, 255, -1, 0);
						PlusMinus (&iFront1703, 638, 318, 0, 255, +1, 0);
						PlusMinus (&iFront1703, 653, 318, 0, 255, +10, 0);
						PlusMinus (&iFront1704, 553, 347, 0, 255, -10, 0);
						PlusMinus (&iFront1704, 568, 347, 0, 255, -1, 0);
						PlusMinus (&iFront1704, 638, 347, 0, 255, +1, 0);
						PlusMinus (&iFront1704, 653, 347, 0, 255, +10, 0);
						PlusMinus (&iFront1705, 553, 376, 0, 255, -10, 0);
						PlusMinus (&iFront1705, 568, 376, 0, 255, -1, 0);
						PlusMinus (&iFront1705, 638, 376, 0, 255, +1, 0);
						PlusMinus (&iFront1705, 653, 376, 0, 255, +10, 0);
						PlusMinus (&iFront1706, 553, 405, 0, 255, -10, 0);
						PlusMinus (&iFront1706, 568, 405, 0, 255, -1, 0);
						PlusMinus (&iFront1706, 638, 405, 0, 255, +1, 0);
						PlusMinus (&iFront1706, 653, 405, 0, 255, +10, 0);
						PlusMinus (&iFront1707, 553, 434, 0, 255, -10, 0);
						PlusMinus (&iFront1707, 568, 434, 0, 255, -1, 0);
						PlusMinus (&iFront1707, 638, 434, 0, 255, +1, 0);
						PlusMinus (&iFront1707, 653, 434, 0, 255, +10, 0);
						PlusMinus (&iFront1708, 553, 463, 0, 255, -10, 0);
						PlusMinus (&iFront1708, 568, 463, 0, 255, -1, 0);
						PlusMinus (&iFront1708, 638, 463, 0, 255, +1, 0);
						PlusMinus (&iFront1708, 653, 463, 0, 255, +10, 0);

						/*** potion ***/
						PlusMinus (&iFront1801, 553, 515, 0, 3, -10, 0);
						PlusMinus (&iFront1801, 568, 515, 0, 3, -1, 0);
						PlusMinus (&iFront1801, 638, 515, 0, 3, +1, 0);
						PlusMinus (&iFront1801, 653, 515, 0, 3, +10, 0);

						/*** loose ***/
						PlusMinus (&iFront1901, 553, 612, 0, 1, -10, 0);
						PlusMinus (&iFront1901, 568, 612, 0, 1, -1, 0);
						PlusMinus (&iFront1901, 638, 612, 0, 1, +1, 0);
						PlusMinus (&iFront1901, 653, 612, 0, 1, +10, 0);

						if ((InArea (2, 2, 2 + 96, 2 + 96) == 1) ||
							(InArea (2, 99, 2 + 96, 99 + 96) == 1) ||
							(InArea (2, 196, 2 + 96, 196 + 96) == 1) ||
							(InArea (2, 293, 2 + 96, 293 + 96) == 1) ||
							(InArea (2, 390, 2 + 96, 390 + 96) == 1) ||
							(InArea (2, 487, 2 + 96, 487 + 96) == 1) ||
							(InArea (2, 584, 2 + 96, 584 + 96) == 1) ||
							(InArea (226, 2, 226 + 96, 2 + 96) == 1) ||
							(InArea (226, 99, 226 + 96, 99 + 96) == 1) ||
							(InArea (226, 196, 226 + 96, 196 + 96) == 1) ||
							(InArea (226, 293, 226 + 96, 293 + 96) == 1) ||
							(InArea (226, 390, 226 + 96, 390 + 96) == 1) ||
							(InArea (226, 487, 226 + 96, 487 + 96) == 1) ||
							(InArea (226, 584, 226 + 96, 584 + 96) == 1) ||
							(InArea (450, 2, 450 + 96, 2 + 96) == 1) ||
							(InArea (450, 99, 450 + 96, 99 + 96) == 1) ||
							(InArea (450, 196, 450 + 96, 196 + 96) == 1) ||
							(InArea (450, 487, 450 + 96, 487 + 96) == 1) ||
							(InArea (450, 584, 450 + 96, 584 + 96) == 1))
						{
							ChangeFrontAction ("select");
						}
					}
					ShowChangeFront();
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_EXPOSED)
						{ ShowChangeFront(); } break;
				case SDL_QUIT:
					Quit(); break;
			}
		}

		/*** prevent CPU eating ***/
		gamespeed = REFRESH_PROG;
		while ((SDL_GetTicks() - looptime) < gamespeed)
		{
			SDL_Delay (10);
		}
		looptime = SDL_GetTicks();
	}
	PlaySound ("wav/ok_close.wav");
	ShowScreen();
}
/*****************************************************************************/
void ShowChangeFront (void)
/*****************************************************************************/
{
	int iOnX, iOnY;
	SDL_Color color;

	ShowImage (imgfront, 0, 0, "imgfront", ascreen, iScale, 1);

	/*** close ***/
	switch (iCloseOn)
	{
		case 0: ShowImage (imgclose[1], 674, 2, "imgclose[1]",
			ascreen, iScale, 1); break; /*** up ***/
		case 1: ShowImage (imgclose[2], 674, 2, "imgclose[2]",
			ascreen, iScale, 1); break; /*** down ***/
	}

	/*** on tile ***/
	switch (iOnTile)
	{
		case 1: iOnX = 2; iOnY = 2; break;
		case 2: iOnX = 2; iOnY = 99; break;
		case 3: iOnX = 2; iOnY = 196; break;
		case 4: iOnX = 2; iOnY = 293; break;
		case 5: iOnX = 2; iOnY = 390; break;
		case 6: iOnX = 2; iOnY = 487; break;
		case 7: iOnX = 2; iOnY = 584; break;
		case 8: iOnX = 226; iOnY = 2; break;
		case 9: iOnX = 226; iOnY = 99; break;
		case 10: iOnX = 226; iOnY = 196; break;
		case 11: iOnX = 226; iOnY = 293; break;
		case 12: iOnX = 226; iOnY = 390; break;
		case 13: iOnX = 226; iOnY = 487; break;
		case 14: iOnX = 226; iOnY = 584; break;
		case 15: iOnX = 450; iOnY = 2; break;
		case 16: iOnX = 450; iOnY = 99; break;
		case 17: iOnX = 450; iOnY = 196; break;
		case 18: iOnX = 450; iOnY = 487; break;
		case 19: iOnX = 450; iOnY = 584; break;
		default:
			printf ("[ WARN ] Unknown tile: %i!\n", iOnTile);
			iOnX = 2; iOnY = 2; /*** Fallback. ***/
	}
	ShowImage (imghoverfront, iOnX, iOnY, "imghoverfront", ascreen, iScale, 1);

	CenterNumber (iFront0101, 133, 30, color_bl, 0);
	CenterNumber (iFront0102, 133, 75, color_bl, 0);
	CenterNumber (iFront0201, 133, 127, color_bl, 0);
	CenterNumber (iFront0202, 133, 172, color_bl, 0);
	CenterNumber (iFront0301, 133, 224, color_bl, 0);
	CenterNumber (iFront0302, 133, 269, color_bl, 0);
	CenterNumber (iFront0401, 133, 321, color_bl, 0);
	CenterNumber (iFront0402, 133, 366, color_bl, 0);
	CenterNumber (iFront0501, 133, 418, color_bl, 0);
	CenterNumber (iFront0502, 133, 463, color_bl, 0);
	CenterNumber (iFront0601, 133, 515, color_bl, 0);
	CenterNumber (iFront0602, 133, 560, color_bl, 0);
	CenterNumber (iFront0701, 133, 612, color_bl, 0);
	CenterNumber (iFront0702, 133, 657, color_bl, 0);
	CenterNumber (iFront1301, 357, 515, color_bl, 0);
	CenterNumber (iFront1401, 357, 612, color_bl, 0);
	CenterNumber (iFront1501, 581, 30, color_bl, 0);
	CenterNumber (iFront1601, 581, 127, color_bl, 0);
	CenterNumber (iFront1701, 581, 224, color_bl, 0);
	if (iFront1702 > 5) { color = color_red; } else { color = color_bl; }
	CenterNumber (iFront1702, 581, 269, color, 0);
	CenterNumber (iFront1703, 581, 318, color_bl, 0);
	CenterNumber (iFront1704, 581, 347, color_bl, 0);
	CenterNumber (iFront1705, 581, 376, color_bl, 0);
	CenterNumber (iFront1706, 581, 405, color_bl, 0);
	CenterNumber (iFront1707, 581, 434, color_bl, 0);
	CenterNumber (iFront1708, 581, 463, color_bl, 0);
	CenterNumber (iFront1801, 581, 515, color_bl, 0);
	CenterNumber (iFront1901, 581, 612, color_bl, 0);

	/*** refresh screen ***/
	SDL_RenderPresent (ascreen);
}
/*****************************************************************************/
void CenterNumber (int iNumber, int iX, int iY,
	SDL_Color fore, int iHex)
/*****************************************************************************/
{
	if (iHex == 0)
	{
		snprintf (arText[0], MAX_TEXT, "%i", iNumber);
	} else {
		snprintf (arText[0], MAX_TEXT, "%02x", iNumber);
	}
	/* The 100000 is a workaround for 0 being broken. SDL devs have fixed that
	 * see e.g. https://hg.libsdl.org/SDL_ttf/rev/72b8861dbc01 but
	 * Ubuntu et al. still ship older sdl2-ttf versions.
	 */
	message = TTF_RenderText_Blended_Wrapped (font[20], arText[0], fore, 100000);
	messaget = SDL_CreateTextureFromSurface (ascreen, message);
	if (iHex == 0)
	{
		if ((iNumber >= -9) && (iNumber <= -1))
		{
			offset.x = iX + 16;
		} else if ((iNumber >= 0) && (iNumber <= 9)) {
			offset.x = iX + 21;
		} else if ((iNumber >= 10) && (iNumber <= 99)) {
			offset.x = iX + 14;
		} else {
			offset.x = iX + 7;
		}
	} else {
		offset.x = iX + 14;
	}
	offset.y = iY - 1;
	offset.w = message->w; offset.h = message->h;
	CustomRenderCopy (messaget, NULL, &offset, "message", 1);
	SDL_DestroyTexture (messaget); SDL_FreeSurface (message);
}
/*****************************************************************************/
int PlusMinus (int *iWhat, int iX, int iY,
	int iMin, int iMax, int iChange, int iAddChanged)
/*****************************************************************************/
{
	if ((InArea (iX, iY, iX + 13, iY + 20) == 1) &&
		(((iChange < 0) && (*iWhat > iMin)) ||
		((iChange > 0) && (*iWhat < iMax))))
	{
		*iWhat = *iWhat + iChange;
		if ((iChange < 0) && (*iWhat < iMin)) { *iWhat = iMin; }
		if ((iChange > 0) && (*iWhat > iMax)) { *iWhat = iMax; }
		if (iAddChanged == 1) { iChanged++; }
		PlaySound ("wav/plus_minus.wav");
		return (1);
	} else { return (0); }
}
/*****************************************************************************/
void LevelSave (void)
/*****************************************************************************/
{
	char sLocation[MAX_PATHFILE + 2];
	int iFd;

	/*** Used for looping. ***/
	int iRowLoop, iColumnLoop;
	int iLoop;
	int iLoopChar;

	if (iChanged == 0) { return; }

	CreateBAK();

	snprintf (sLocation, MAX_PATHFILE, "%s%s%i.lvl",
		DIR_UNCOMP, SLASH, iCurLevel);
	iFd = open (sLocation, O_WRONLY|O_TRUNC|O_CREAT|O_BINARY, 0600);
	if (iFd == -1)
	{
		printf ("[FAILED] Could not open \"%s\": %s!\n",
			sPathFile, strerror (errno));
		exit (EXIT_ERROR);
	}

	/*** width and height ***/
	WriteByte (iFd, ulWidth);
	WriteByte (iFd, ulHeight);

	/*** back ***/
	for (iRowLoop = 1; iRowLoop <= (int)ulHeight; iRowLoop++)
	{
		for (iColumnLoop = 1; iColumnLoop <= (int)ulWidth; iColumnLoop++)
		{
			WriteByte (iFd, ulBack[iRowLoop][iColumnLoop]);
		}
	}

	/*** unknown ***/
	WriteWord (iFd, ulNrUnknown);
	for (iLoop = 1; iLoop <= (int)ulNrUnknown; iLoop++)
	{
		WriteByte (iFd, ulUnknownA[iLoop]);
		WriteByte (iFd, ulUnknownB[iLoop]);
		WriteByte (iFd, ulUnknownC[iLoop]);
		WriteByte (iFd, ulUnknownD[iLoop]);
	}

	/*** 0x00 0x00 ***/
	WriteByte (iFd, 0);
	WriteByte (iFd, 0);

	/*** front types ***/
	WriteWord (iFd, ulFrontTypes);
	for (iLoop = 1; iLoop <= (int)ulFrontTypes; iLoop++)
	{
		WriteByte (iFd, ulFrontTypeA[iLoop]);
		WriteByte (iFd, ulFrontTypeB[iLoop]);
		WriteByte (iFd, ulFrontTypeNr[iLoop]);
	}

	/*** front ***/
	WriteWord (iFd, ulNrFront);
	for (iLoop = 1; iLoop <= (int)ulNrFront; iLoop++)
	{
		WriteWord (iFd, ulFrontX[iLoop]);
		WriteWord (iFd, ulFrontY[iLoop]);
		WriteByte (iFd, ulFrontType[iLoop]);
		WriteByte (iFd, ulFrontA[iLoop]);
		WriteByte (iFd, ulFrontB[iLoop]);
	}

	/*** prince ***/
	WriteWord (iFd, ulPrinceX);
	WriteWord (iFd, ulPrinceY);

	/*** exit trigger ***/
	WriteWord (iFd, ulExitTriggerX);
	WriteWord (iFd, ulExitTriggerY);

	/*** save ***/
	WriteWord (iFd, ulSaveTriggerX);
	WriteWord (iFd, ulSaveTriggerY);

	/*** entrance image ***/
	WriteWord (iFd, ulEntranceImageX);
	WriteWord (iFd, ulEntranceImageY);

	/*** exit image ***/
	WriteWord (iFd, ulExitImageX);
	WriteWord (iFd, ulExitImageY);

	/*** chompers ***/
	WriteWord (iFd, ulNrChompers);
	for (iLoop = 1; iLoop <= (int)ulNrChompers; iLoop++)
	{
		WriteWord (iFd, ulChomperX[iLoop]);
		WriteWord (iFd, ulChomperY[iLoop]);
		WriteByte (iFd, ulChomperA[iLoop]);
	}

	/*** spikes ***/
	WriteWord (iFd, ulNrSpikes);
	for (iLoop = 1; iLoop <= (int)ulNrSpikes; iLoop++)
	{
		WriteWord (iFd, ulSpikeX[iLoop]);
		WriteWord (iFd, ulSpikeY[iLoop]);
		WriteByte (iFd, ulSpikeRight[iLoop]);
	}

	/*** gates ***/
	WriteWord (iFd, ulNrGates);
	for (iLoop = 1; iLoop <= (int)ulNrGates; iLoop++)
	{
		WriteWord (iFd, ulGateX[iLoop]);
		WriteWord (iFd, ulGateY[iLoop]);
		WriteWord (iFd, ulGateTimeOpen[iLoop]);
	}

	/*** raise ***/
	WriteWord (iFd, ulNrRaise);
	for (iLoop = 1; iLoop <= (int)ulNrRaise; iLoop++)
	{
		WriteWord (iFd, ulRaiseGate[iLoop]);
		WriteWord (iFd, ulRaiseX[iLoop]);
		WriteWord (iFd, ulRaiseY[iLoop]);
	}

	/*** guards ***/
	WriteWord (iFd, ulNrGuards);
	for (iLoop = 1; iLoop <= (int)ulNrGuards; iLoop++)
	{
		WriteWord (iFd, ulGuardX[iLoop]);
		WriteWord (iFd, ulGuardY[iLoop]);
		WriteByte (iFd, ulGuardDir[iLoop]);
		WriteByte (iFd, ulGuardHP[iLoop]);
		WriteByte (iFd, ulGuardA[iLoop]);
		WriteByte (iFd, ulGuardB[iLoop]);
		WriteByte (iFd, ulGuardC[iLoop]);
		WriteByte (iFd, ulGuardD[iLoop]);
		WriteByte (iFd, ulGuardE[iLoop]);
		WriteByte (iFd, ulGuardF[iLoop]);
	}

	/*** potions ***/
	WriteWord (iFd, ulNrPotions);
	for (iLoop = 1; iLoop <= (int)ulNrPotions; iLoop++)
	{
		WriteByte (iFd, ulPotionType[iLoop]);
		WriteWord (iFd, ulPotionX[iLoop]);
		WriteWord (iFd, ulPotionY[iLoop]);
	}

	/*** loose ***/
	WriteWord (iFd, ulNrLoose);
	for (iLoop = 1; iLoop <= (int)ulNrLoose; iLoop++)
	{
		WriteWord (iFd, ulLooseX[iLoop]);
		WriteWord (iFd, ulLooseY[iLoop]);
		WriteByte (iFd, ulLooseRight[iLoop]);
	}

	/*** 0x00 0x00 ***/
	WriteByte (iFd, 0);
	WriteByte (iFd, 0);

	/*** text ***/
	ulNrText = 0;
	for (iLoop = 1; iLoop <= MAX_LINES; iLoop++)
	{
		if (strcmp (arTextLine[iLoop], "") != 0)
		{
			ulNrText += strlen (arTextLine[iLoop]) + 1; /*** + 1 for '\' ***/
		}
	}
	WriteWord (iFd, ulNrText);
	for (iLoop = 1; iLoop <= MAX_LINES; iLoop++)
	{
		if (strcmp (arTextLine[iLoop], "") != 0)
		{
			for (iLoopChar = 0; iLoopChar < (int)strlen (arTextLine[iLoop]);
				iLoopChar++)
			{
				WriteByte (iFd, arTextLine[iLoop][iLoopChar]);
			}
			WriteByte (iFd, 0x5C); /*** '\' ***/
		}
	}

	close (iFd);

	Compress();

	PlaySound ("wav/save.wav");

	iChanged = 0;
}
/*****************************************************************************/
void CreateBAK (void)
/*****************************************************************************/
{
	FILE *fDAT;
	FILE *fBAK;
	int iData;

	fDAT = fopen (sPathFile, "rb");
	if (fDAT == NULL)
		{ printf ("[FAILED] Could not open %s: %s!\n",
			sPathFile, strerror (errno)); }

	fBAK = fopen (BACKUP, "wb");
	if (fBAK == NULL)
		{ printf ("[FAILED] Could not open %s: %s!\n",
			BACKUP, strerror (errno)); }

	while (1)
	{
		iData = fgetc (fDAT);
		if (iData == EOF) { break; }
			else { putc (iData, fBAK); }
	}

	fclose (fDAT);
	fclose (fBAK);
}
/*****************************************************************************/
void WriteByte (int iFd, int iValue)
/*****************************************************************************/
{
	char sToWrite[MAX_TOWRITE + 2];
	int iWritten;

	snprintf (sToWrite, MAX_TOWRITE, "%c", iValue);
	iWritten = write (iFd, sToWrite, 1);
	if (iWritten == -1)
	{
		snprintf (sWarning, MAX_WARNING, "Could not write: %s!",
			strerror (errno));
		Warning();
	}
}
/*****************************************************************************/
void WriteWord (int iFd, int iValue)
/*****************************************************************************/
{
	char sToWrite[MAX_TOWRITE + 2];
	int iWritten;

	snprintf (sToWrite, MAX_TOWRITE, "%c%c", (iValue >> 0) & 0xFF,
		(iValue >> 8) & 0xFF);
	iWritten = write (iFd, sToWrite, 2);
	if (iWritten == -1)
	{
		snprintf (sWarning, MAX_WARNING, "Could not write: %s!",
			strerror (errno));
		Warning();
	}
}
/*****************************************************************************/
void PopUpSave (void)
/*****************************************************************************/
{
	int iSave;
	SDL_Event event;

	iSave = 1;

	PlaySound ("wav/popup_yn.wav");
	ShowPopUpSave();
	while (iSave == 1)
	{
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_CONTROLLERBUTTONDOWN:
					/*** Nothing for now. ***/
					break;
				case SDL_CONTROLLERBUTTONUP:
					switch (event.cbutton.button)
					{
						case SDL_CONTROLLER_BUTTON_A:
							LevelSave(); iSave = 0; break;
						case SDL_CONTROLLER_BUTTON_B:
							iSave = 0; break;
					}
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
						case SDLK_n:
							iSave = 0; break;
						case SDLK_y:
							LevelSave(); iSave = 0; break;
					}
					break;
				case SDL_MOUSEMOTION:
					iXPos = event.motion.x;
					iYPos = event.motion.y;
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 1)
					{
						if (InArea (450, 428, 450 + 85, 428 + 32) == 1) /*** Yes ***/
							{ iYesOn = 1; }
						if (InArea (177, 428, 177 + 85, 428 + 32) == 1) /*** No ***/
							{ iNoOn = 1; }
					}
					ShowPopUpSave();
					break;
				case SDL_MOUSEBUTTONUP:
					iYesOn = 0;
					iNoOn = 0;
					if (event.button.button == 1)
					{
						if (InArea (450, 428, 450 + 85, 428 + 32) == 1) /*** Yes ***/
							{ LevelSave(); iSave = 0; }
						if (InArea (177, 428, 177 + 85, 428 + 32) == 1) /*** No ***/
							{ iSave = 0; }
					}
					ShowPopUpSave();
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_EXPOSED)
						{ ShowScreen(); ShowPopUpSave(); } break;
				case SDL_QUIT:
					Quit(); break;
			}
		}

		/*** prevent CPU eating ***/
		gamespeed = REFRESH_PROG;
		while ((SDL_GetTicks() - looptime) < gamespeed)
		{
			SDL_Delay (10);
		}
		looptime = SDL_GetTicks();
	}
	PlaySound ("wav/popup_close.wav");
	ShowScreen();
}
/*****************************************************************************/
void ShowPopUpSave (void)
/*****************************************************************************/
{
	/*** faded background ***/
	ShowImage (imgfaded, 0, 0, "imgfaded", ascreen, iScale, 1);

	/*** popup ***/
	ShowImage (imgpopup_yn, 160, 202, "imgpopup_yn", ascreen, iScale, 1);

	/*** Yes ***/
	switch (iYesOn)
	{
		case 0: ShowImage (imgyes[1], 450, 428, "imgyes[1]",
			ascreen, iScale, 1); break; /*** off ***/
		case 1: ShowImage (imgyes[2], 450, 428, "imgyes[2]",
			ascreen, iScale, 1); break; /*** on ***/
	}

	/*** No ***/
	switch (iNoOn)
	{
		case 0: ShowImage (imgno[1], 177, 428, "imgno[1]",
			ascreen, iScale, 1); break; /*** off ***/
		case 1: ShowImage (imgno[2], 177, 428, "imgno[2]",
			ascreen, iScale, 1); break; /*** on ***/
	}

	if (iChanged == 1)
	{
		snprintf (arText[0], MAX_TEXT, "%s", "You made an unsaved change.");
		snprintf (arText[1], MAX_TEXT, "%s", "Do you want to save it?");
	} else {
		snprintf (arText[0], MAX_TEXT, "%s", "There are unsaved changes.");
		snprintf (arText[1], MAX_TEXT, "%s", "Do you wish to save these?");
	}

	DisplayText (190, 231, 15, 2, color_wh, 1);

	/*** refresh screen ***/
	SDL_RenderPresent (ascreen);
}
/*****************************************************************************/
void Zoom (int iToggleFull)
/*****************************************************************************/
{
	/*** Used for looping. ***/
	int iLoopFont;

	if (iToggleFull == 1)
	{
		if (iFullscreen == 0)
			{ iFullscreen = SDL_WINDOW_FULLSCREEN_DESKTOP; }
				else { iFullscreen = 0; }
	} else {
		if (iFullscreen == SDL_WINDOW_FULLSCREEN_DESKTOP)
		{
			iFullscreen = 0;
			iScale = 1;
		} else if (iScale == 1) {
			iScale = 2;
		} else if (iScale == 2) {
			iFullscreen = SDL_WINDOW_FULLSCREEN_DESKTOP;
		} else {
			printf ("[ WARN ] Unknown window state!\n");
		}
	}

	SDL_SetWindowFullscreen (window, iFullscreen);
	SDL_SetWindowSize (window, (WINDOW_WIDTH) * iScale,
		(WINDOW_HEIGHT) * iScale);
	SDL_RenderSetLogicalSize (ascreen, (WINDOW_WIDTH) * iScale,
		(WINDOW_HEIGHT) * iScale);
	SDL_SetWindowPosition (window, SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED);
	for (iLoopFont = 1; iLoopFont <= MAX_FONT_SIZE; iLoopFont++)
		 { TTF_CloseFont (font[iLoopFont]); }
	LoadFonts();

	PlaySound ("wav/extras.wav");
}
/*****************************************************************************/
void Compress (void)
/*****************************************************************************/
{
	int iError;
	char sFrom[MAX_PATHFILE + 2];
	char sZipError[MAX_ERROR + 2];
	DIR *dDir;
	struct dirent *stDirent;
	struct stat stStat;

	zip = zip_open (sPathFile, ZIP_CREATE|ZIP_TRUNCATE, &iError);
	if (zip == NULL)
	{
		zip_error_to_str (sZipError, sizeof (sZipError), iError, errno);
		snprintf (sWarning, MAX_WARNING, "Cannot create \"%s\": %s!",
			sPathFile, sZipError);
		Warning();
	} else {
		ZIPDir ("META-INF");
		snprintf (sFrom, MAX_PATHFILE, "%s%sMETA-INF%sMANIFEST.MF",
			DIR_UNCOMP, SLASH, SLASH);
		if (access (sFrom, R_OK) == -1) { CreateMeta (sFrom); }
		ZIPFile (sFrom, "META-INF/MANIFEST.MF");
		dDir = opendir (DIR_UNCOMP);
		if (dDir == NULL)
		{
			snprintf (sWarning, MAX_WARNING, "Cannot open directory \"%s\": %s!",
				DIR_UNCOMP, strerror (errno));
			Warning();
		} else {
			while ((stDirent = readdir (dDir)) != NULL)
			{
				if ((strcmp (stDirent->d_name, ".") != 0) &&
					(strcmp (stDirent->d_name, "..") != 0) &&
					(strcmp (stDirent->d_name, "META-INF") != 0))
				{
					stat (stDirent->d_name, &stStat);
					if (S_ISDIR (stStat.st_mode))
					{
						ZIPDir (stDirent->d_name);
						/*** Maybe also add its files. ***/
					} else {
						snprintf (sFrom, MAX_PATHFILE, "%s%s%s",
							DIR_UNCOMP, SLASH, stDirent->d_name);
						ZIPFile (sFrom, stDirent->d_name);
					}
				}
			}
			PrIfDe ("[  OK  ] Compressed JAR file.\n");
		}
		zip_close (zip);
	}
}
/*****************************************************************************/
int ZIPDir (char *sDir)
/*****************************************************************************/
{
	int iResult;
	int iReturn;

	iResult = zip_dir_add (zip, sDir, 0);
	if (iResult == -1)
	{
		snprintf (sWarning, MAX_WARNING, "Could not add %s/: %s!",
			sDir, zip_strerror (zip));
		Warning();
		iReturn = -1;
	} else { iReturn = 0; }

	return (iReturn);
}
/*****************************************************************************/
int ZIPFile (char *sFrom, char *sTo)
/*****************************************************************************/
{
	zip_source_t *s;
	int iResult;
	int iReturn;

	if (access (sFrom, R_OK) == -1)
	{
		snprintf (sWarning, MAX_WARNING, "Could not read %s: %s!",
			sFrom, strerror (errno));
		Warning();
		iReturn = 1;
	} else {
		s = zip_source_file (zip, sFrom, 0, 0);
		if (s == NULL)
		{
			snprintf (sWarning, MAX_WARNING, "Could not read %s: %s!",
				sFrom, zip_strerror (zip));
			Warning();
			iReturn = -1;
		} else {
			iResult = zip_file_add (zip, sTo, s, 0);
			if (iResult == -1)
			{
				snprintf (sWarning, MAX_WARNING, "Could not add %s: %s!",
					sFrom, zip_strerror (zip));
				Warning();
				iReturn = -1;
			} else { iReturn = 0; }
			/*** Do NOT call zip_source_free(). ***/
		}
	}

	return (iReturn);
}
/*****************************************************************************/
void Playtest (void)
/*****************************************************************************/
{
	SDL_Thread *princethread;

	princethread = SDL_CreateThread (StartGame, "StartGame", NULL);
	if (princethread == NULL)
		{ printf ("[ WARN ] Could not create thread!\n"); }
}
/*****************************************************************************/
int StartGame (void *unused)
/*****************************************************************************/
{
	char sSystem[200 + 2];

	if (unused != NULL) { } /*** To prevent warnings. ***/

	PlaySound ("wav/emulator.wav");

	snprintf (sSystem, 200, "java -jar emulator%smicroemulator.jar %s > %s",
		SLASH, sPathFile, DEVNULL);
	if (system (sSystem) == -1)
	{
		snprintf (sWarning, MAX_WARNING, "%s",
			"Could not execute emulator! Is Java installed?");
		Warning();
	}

	return (EXIT_NORMAL);
}
/*****************************************************************************/
void AddFront (int iX, int iY, int iFrontTypeNr)
/*****************************************************************************/
{
	int iType;
	int iLoop;

	if (iDebug == 1)
	{
		printf ("[ INFO ] Will add front type 0x%02X to x:%i, y:%i.\n",
			iFrontTypeNr, iX, iY);
	}

	iType = -1;
	for (iLoop = 1; iLoop <= (int)ulFrontTypes; iLoop++)
	{
		if ((int)ulFrontTypeNr[iLoop] == iFrontTypeNr) { iType = iLoop - 1; }
	}
	if (iType == -1)
	{
		ulFrontTypes++;
		ulFrontTypeA[ulFrontTypes] = 0x01;
		ulFrontTypeB[ulFrontTypes] = 0x00;
		ulFrontTypeNr[ulFrontTypes] = iFrontTypeNr;
		iType = ulFrontTypes - 1;
		if (iDebug == 1)
		{
			printf ("[  OK  ] Added front type 0x%02X as %i.\n",
				iFrontTypeNr, iType);
		}
	} else {
		if (iDebug == 1)
		{
			printf ("[ INFO ] Front type 0x%02X found as %i.\n",
				iFrontTypeNr, iType);
		}
	}
	ulNrFront++;
	ulFrontX[ulNrFront] = iX;
	ulFrontY[ulNrFront] = iY;
	ulFrontType[ulNrFront] = iType;
	switch (iFrontTypeNr)
	{
		case 0x30: /*** torch ***/
			ulFrontA[ulNrFront] = iFront0101;
			ulFrontB[ulNrFront] = iFront0102;
			break;
		case 0x31: /*** pillar front ***/
			ulFrontA[ulNrFront] = iFront0201;
			ulFrontB[ulNrFront] = iFront0202;
			break;
		case 0x32: /*** skeleton ***/
			ulFrontA[ulNrFront] = iFront0301;
			ulFrontB[ulNrFront] = iFront0302;
			break;
		case 0x33: /*** wall top left slash ***/
			ulFrontA[ulNrFront] = iFront0401;
			ulFrontB[ulNrFront] = iFront0402;
			break;
		case 0x34: /*** wall top left dot ***/
			ulFrontA[ulNrFront] = iFront0501;
			ulFrontB[ulNrFront] = iFront0502;
			break;
		case 0x35: /*** wall bottom left ***/
			ulFrontA[ulNrFront] = iFront0601;
			ulFrontB[ulNrFront] = iFront0602;
			break;
		case 0x36: /*** floor climbable ***/
			ulFrontA[ulNrFront] = iFront0701;
			ulFrontB[ulNrFront] = iFront0702;
			break;
	}
}
/*****************************************************************************/
void ClearLevel (void)
/*****************************************************************************/
{
	/*** Used for looping. ***/
	int iRowLoop, iColumnLoop;

	for (iRowLoop = 1; iRowLoop <= (int)ulHeight; iRowLoop++)
	{
		for (iColumnLoop = 1; iColumnLoop <= (int)ulWidth; iColumnLoop++)
			{ ulBack[iRowLoop][iColumnLoop] = 0x04; }
	}
	/*** Not touching ulNrUnknown. ***/
	/*** Not touching ulFrontTypes. ***/
	ulNrFront = 1; /*** Should not be 0, to prevent level start hang. ***/
	ulNrChompers = 0;
	ulNrSpikes = 1; /*** Should not be 0, to prevent fall hang. ***/
	ulNrGates = 1; /*** Should not be 0, to prevent error messages. ***/
	ulNrRaise = 0;
	ulNrGuards = 0;
	ulNrPotions = 1; /*** Must not be 0, to prevent level start hang. ***/
	ulNrLoose = 0;

	PlaySound ("wav/ok_close.wav");
	iChanged++;
}
/*****************************************************************************/
void Text (void)
/*****************************************************************************/
{
	int iText;
	SDL_Event event;
	char cAdd;
	char sTempLine[MAX_LINECHARS + 2];

	/*** Used for looping. ***/
	int iLoopLine;

	iText = 1;
	iLine = 0;

	PlaySound ("wav/popup.wav");
	ShowText();
	while (iText == 1)
	{
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_CONTROLLERBUTTONDOWN:
					/*** Nothing for now. ***/
					break;
				case SDL_CONTROLLERBUTTONUP:
					switch (event.cbutton.button)
					{
						case SDL_CONTROLLER_BUTTON_A:
							iText = 0; break;
					}
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
						case SDLK_KP_ENTER:
						case SDLK_RETURN:
							iText = 0; break;
						case SDLK_SPACE:
						case SDLK_o:
							if (iLine == 0) { iText = 0; } break;
						case SDLK_BACKSPACE:
							if ((iLine != 0) && (strlen (arTextLine[iLine]) > 0))
							{
								arTextLine[iLine][strlen (arTextLine[iLine]) - 1] = '\0';
								PlaySound ("wav/hum_adj.wav");
							}
							break;
					}
					ShowText();
					break;
				case SDL_MOUSEMOTION:
					iXPos = event.motion.x;
					iYPos = event.motion.y;

					/*** stop/start input ***/
					iLine = 0;
					for (iLoopLine = 1; iLoopLine <= MAX_LINES; iLoopLine++)
					{
						if (InArea (51, 51 + (iLoopLine * 42), 51 + 400,
							51 + (iLoopLine * 42) + 30) == 1) { iLine = iLoopLine; }
					}
					if (iLine == 0)
					{
						SDL_SetCursor (curArrow);
						SDL_StopTextInput();
					} else {
						SDL_SetCursor (curText);
						SDL_StartTextInput();
					}

					ShowText();
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 1)
					{
						if (InArea (610, 633, 610 + 85, 633 + 32) == 1) /*** OK ***/
							{ iTextOK = 1; }
					}
					ShowText();
					break;
				case SDL_MOUSEBUTTONUP:
					iTextOK = 0;
					if (event.button.button == 1)
					{
						if (InArea (610, 633, 610 + 85, 633 + 32) == 1) /*** OK ***/
							{ iText = 0; }
					}
					ShowText();
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_EXPOSED)
						{ ShowText(); } break;
				case SDL_QUIT:
					Quit(); break;
				case SDL_TEXTINPUT:
					if ((iLine != 0) && (strlen (arTextLine[iLine]) < MAX_LINECHARS - 2))
					{
						cAdd = event.text.text[0];
						if (((cAdd >= 'a') && (cAdd <= 'z')) ||
							((cAdd >= 'A') && (cAdd <= 'Z')) ||
							(cAdd == ' ') || (cAdd == '\'') ||
							(cAdd == '!') || (cAdd == '.'))
						{
							snprintf (sTempLine, MAX_LINECHARS, "%s", arTextLine[iLine]);
							snprintf (arTextLine[iLine], MAX_LINECHARS, "%s%c",
								sTempLine, cAdd);
							PlaySound ("wav/hum_adj.wav");
							iChanged++;
						}
					}
					ShowText();
					break;
			}
		}

		/*** prevent CPU eating ***/
		gamespeed = REFRESH_PROG;
		while ((SDL_GetTicks() - looptime) < gamespeed)
		{
			SDL_Delay (10);
		}
		looptime = SDL_GetTicks();
	}
	PlaySound ("wav/popup_close.wav");
	SDL_SetCursor (curArrow);
	SDL_StopTextInput();
	ShowScreen();
}
/*****************************************************************************/
void ShowText (void)
/*****************************************************************************/
{
	/*** Used for looping. ***/
	int iLoopLine;

	/*** background ***/
	ShowImage (imgtext, 0, 0, "imgtext", ascreen, iScale, 1);

	/*** OK ***/
	switch (iTextOK)
	{
		case 0: ShowImage (imgok[1], 610, 633, "imgok[1]",
			ascreen, iScale, 1); break; /*** up ***/
		case 1: ShowImage (imgok[2], 610, 633, "imgok[2]",
			ascreen, iScale, 1); break; /*** down ***/
	}

	/*** text ***/
	for (iLoopLine = 1; iLoopLine <= MAX_LINES; iLoopLine++)
	{
		snprintf (arText[0], MAX_TEXT, "%s", arTextLine[iLoopLine]);
		DisplayText (56, 55 + (iLoopLine * 42), 20, 1, color_wh, 1);
	}

	/*** hover ***/
	if (iLine != 0)
	{
		ShowImage (imghovertext, 51, 51 + (iLine * 42), "imghovertext",
			ascreen, iScale, 1);
	}

	/*** refresh screen ***/
	SDL_RenderPresent (ascreen);
}
/*****************************************************************************/
void EXE (void)
/*****************************************************************************/
{
	int iEXE;
	SDL_Event event;
	int iOldXPos, iOldYPos;
	int iEXEOld;

	iEXE = 1;
	iStatusBarFrame = 1;
	snprintf (sStatus, MAX_STATUS, "%s", "");

	EXELoad();

	PlaySound ("wav/popup.wav");
	ShowEXE();
	while (iEXE == 1)
	{
		if (iNoAnim == 0)
		{
			/*** We use the global REFRESH. No need for newticks/oldticks. ***/
			iStatusBarFrame++;
			if (iStatusBarFrame == 19) { iStatusBarFrame = 1; }
			ShowEXE();
		}

		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_CONTROLLERBUTTONDOWN:
					/*** Nothing for now. ***/
					break;
				case SDL_CONTROLLERBUTTONUP:
					switch (event.cbutton.button)
					{
						case SDL_CONTROLLER_BUTTON_A:
							EXESave(); iEXE = 0; break;
						case SDL_CONTROLLER_BUTTON_B:
							iEXE = 0; break;
					}
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							iEXE = 0; break;
						case SDLK_KP_ENTER:
						case SDLK_RETURN:
						case SDLK_SPACE:
						case SDLK_s:
							EXESave(); iEXE = 0;
							break;
					}
					ShowEXE();
					break;
				case SDL_MOUSEMOTION:
					iOldXPos = iXPos;
					iOldYPos = iYPos;
					iXPos = event.motion.x;
					iYPos = event.motion.y;
					if ((iOldXPos == iXPos) && (iOldYPos == iYPos)) { break; }

					UpdateStatusBar();
					ShowEXE();
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 1)
					{
						if (InArea (610, 633, 610 + 85, 633 + 32) == 1) /*** Save ***/
							{ iEXESave = 1; }
					}
					ShowEXE();
					break;
				case SDL_MOUSEBUTTONUP:
					iEXESave = 0;
					if (event.button.button == 1)
					{
						/*** Menu, initial selection ***/
						iEXEOld = iEXEMenuInitialSelection;
						PlusMinus (&iEXEMenuInitialSelection, 565, 34, 1, 8, -10, 0);
						PlusMinus (&iEXEMenuInitialSelection, 580, 34, 1, 8, -1, 0);
						PlusMinus (&iEXEMenuInitialSelection, 650, 34, 1, 8, +1, 0);
						PlusMinus (&iEXEMenuInitialSelection, 665, 34, 1, 8, +10, 0);
						if (iEXEMenuInitialSelection != iEXEOld) { UpdateStatusBar(); }

						/*** Cutscenes, font emphasis ***/
						iEXEOld = iEXECutscenesFontEmphasis;
						PlusMinus (&iEXECutscenesFontEmphasis, 565, 58, 2, 8, -10, 0);
						PlusMinus (&iEXECutscenesFontEmphasis, 580, 58, 2, 8, -1, 0);
						PlusMinus (&iEXECutscenesFontEmphasis, 650, 58, 2, 8, +1, 0);
						PlusMinus (&iEXECutscenesFontEmphasis, 665, 58, 2, 8, +10, 0);
						if (iEXECutscenesFontEmphasis != iEXEOld) { UpdateStatusBar(); }

						/*** Cutscenes, text lines ***/
						iEXEOld = iEXECutscenesTextLines;
						PlusMinus (&iEXECutscenesTextLines, 565, 82, 2, 5, -10, 0);
						PlusMinus (&iEXECutscenesTextLines, 580, 82, 2, 5, -1, 0);
						PlusMinus (&iEXECutscenesTextLines, 650, 82, 2, 5, +1, 0);
						PlusMinus (&iEXECutscenesTextLines, 665, 82, 2, 5, +10, 0);
						if (iEXECutscenesTextLines != iEXEOld) { UpdateStatusBar(); }

						if (InArea (610, 633, 610 + 85, 633 + 32) == 1) /*** Save ***/
							{ EXESave(); iEXE = 0; }
					}
					ShowEXE();
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_EXPOSED)
						{ ShowEXE(); } break;
				case SDL_QUIT:
					Quit(); break;
			}
		}

		/*** prevent CPU eating ***/
		gamespeed = REFRESH_PROG;
		while ((SDL_GetTicks() - looptime) < gamespeed)
		{
			SDL_Delay (10);
		}
		looptime = SDL_GetTicks();
	}
	PlaySound ("wav/popup_close.wav");
	ShowScreen();
}
/*****************************************************************************/
void ShowEXE (void)
/*****************************************************************************/
{
	SDL_Color color;

	/*** background ***/
	ShowImage (imgexe, 0, 0, "imgexe", ascreen, iScale, 1);

	/*** Menu, initial selection ***/
	switch (iEXEMenuInitialSelection)
	{
		case 1: case 2: color = color_red; break;
		case 3: color = color_bl; break;
		default: color = color_blue; break;
	}
	CenterNumber (iEXEMenuInitialSelection, 593, 34, color, 0);

	/*** Cutscenes, font emphasis ***/
	if (iEXECutscenesFontEmphasis == 3)
		{ color = color_bl; } else { color = color_blue; }
	CenterNumber (iEXECutscenesFontEmphasis, 593, 58, color, 0);

	/*** Cutscenes, text lines ***/
	if (iEXECutscenesTextLines == 3)
		{ color = color_bl; } else { color = color_blue; }
	CenterNumber (iEXECutscenesTextLines, 593, 82, color, 0);

	/*** status bar ***/
	if (strcmp (sStatus, "") != 0)
	{
		/*** bulb ***/
		ShowImage (imgstatusbarsprite, 23, 639, "imgstatusbarsprite",
			ascreen, iScale, 1);
		/*** text ***/
		snprintf (arText[0], MAX_TEXT, "%s", sStatus);
		DisplayText (50, 643, 11, 1, color_f4, 1);
	}

	/*** Save ***/
	switch (iEXESave)
	{
		case 0: ShowImage (imgsave[1], 610, 633, "imgsave[1]",
			ascreen, iScale, 1); break; /*** up ***/
		case 1: ShowImage (imgsave[2], 610, 633, "imgsave[2]",
			ascreen, iScale, 1); break; /*** down ***/
	}

	/*** refresh screen ***/
	SDL_RenderPresent (ascreen);
}
/*****************************************************************************/
int OpenClass (char *sClass, char cReadOrWrite)
/*****************************************************************************/
{
	char sLocation[MAX_PATHFILE + 2];
	int iFd;

	snprintf (sLocation, MAX_PATHFILE, "%s%s%s", DIR_UNCOMP, SLASH, sClass);
	switch (cReadOrWrite)
	{
		case 'r': iFd = open (sLocation, O_RDONLY|O_BINARY); break;
		case 'w': iFd = open (sLocation, O_WRONLY|O_BINARY); break;
		default:
			snprintf (sError, MAX_ERROR, "Unknown open() char: %c!", cReadOrWrite);
			ErrorAndExit(); break;
	}
	if (iFd == -1)
	{
		snprintf (sError, MAX_ERROR, "Could not open \"%s\": %s!",
			sLocation, strerror (errno));
		ErrorAndExit();
	}

	return (iFd);
}
/*****************************************************************************/
void EXELoad (void)
/*****************************************************************************/
{
	int iFdEXE;
	char sRead[1 + 2];

	iFdEXE = OpenClass ("F.class", 'r');

	/*** Menu, initial selection ***/
	lseek (iFdEXE, 0x4ED6, SEEK_SET);
	read (iFdEXE, sRead, 1);
	iEXEMenuInitialSelection = sRead[0];

	/*** Cutscenes, font emphasis ***/
	lseek (iFdEXE, 0x5AFA, SEEK_SET);
	read (iFdEXE, sRead, 1);
	iEXECutscenesFontEmphasis = sRead[0];

	/*** Cutscenes, text lines ***/
	lseek (iFdEXE, 0x5AE6, SEEK_SET);
	read (iFdEXE, sRead, 1);
	iEXECutscenesTextLines = sRead[0];

	close (iFdEXE);
}
/*****************************************************************************/
void EXESave (void)
/*****************************************************************************/
{
	int iFdEXE;

	iFdEXE = OpenClass ("F.class", 'w');

	/*** Menu, initial selection ***/
	lseek (iFdEXE, 0x4ED6, SEEK_SET);
	WriteByte (iFdEXE, iEXEMenuInitialSelection);

	/*** Cutscenes, font emphasis ***/
	lseek (iFdEXE, 0x5AFA, SEEK_SET);
	WriteByte (iFdEXE, iEXECutscenesFontEmphasis);

	/*** Cutscenes, text lines ***/
	lseek (iFdEXE, 0x5AE6, SEEK_SET);
	WriteByte (iFdEXE, iEXECutscenesTextLines);

	close (iFdEXE);

	Compress();

	PlaySound ("wav/save.wav");
}
/*****************************************************************************/
void UpdateStatusBar (void)
/*****************************************************************************/
{
	snprintf (sStatusOld, MAX_STATUS, "%s", sStatus);
	snprintf (sStatus, MAX_STATUS, "%s", "");

	/*** Menu, initial selection ***/
	if (InArea (412, 34, 412 + 266, 34 + 20) == 1)
	{
		switch (iEXEMenuInitialSelection)
		{
			case 1: snprintf (sStatus, MAX_STATUS, "%s",
				"1 = Controls [buggy]"); break;
			case 2: snprintf (sStatus, MAX_STATUS, "%s",
				"2 = Continue [buggy]"); break;
			case 3: snprintf (sStatus, MAX_STATUS, "%s",
				"3 = New Game"); break;
			case 4: snprintf (sStatus, MAX_STATUS, "%s",
				"4 = Exit"); break;
			case 5: snprintf (sStatus, MAX_STATUS, "%s",
				"5 = Vibration"); break;
			case 6: snprintf (sStatus, MAX_STATUS, "%s",
				"6 = Sound"); break;
			case 7: snprintf (sStatus, MAX_STATUS, "%s",
				"7 = Tips"); break;
			case 8: snprintf (sStatus, MAX_STATUS, "%s",
				"8 = About"); break;
		}
	}

	/*** Cutscenes, font emphasis ***/
	if (InArea (384, 58, 384 + 294, 58 + 20) == 1)
	{
		switch (iEXECutscenesFontEmphasis)
		{
			case 2: snprintf (sStatus, MAX_STATUS, "%s",
				"2 = bold, italic, underline"); break;
			case 3: snprintf (sStatus, MAX_STATUS, "%s",
				"3 = (none)"); break;
			case 4: snprintf (sStatus, MAX_STATUS, "%s",
				"4 = bold"); break;
			case 5: snprintf (sStatus, MAX_STATUS, "%s",
				"5 = italic"); break;
			case 6: snprintf (sStatus, MAX_STATUS, "%s",
				"6 = bold, italic"); break;
			case 7: snprintf (sStatus, MAX_STATUS, "%s",
				"7 = underline"); break;
			case 8: snprintf (sStatus, MAX_STATUS, "%s",
				"8 = bold, underline"); break;
		}
	}

	/*** Cutscenes, text lines ***/
	if (InArea (418, 82, 418 + 260, 82 + 20) == 1)
	{
		switch (iEXECutscenesTextLines)
		{
			case 2: snprintf (sStatus, MAX_STATUS, "%s",
				"2 = four lines"); break;
			case 3: snprintf (sStatus, MAX_STATUS, "%s",
				"3 = three lines"); break;
			case 4: snprintf (sStatus, MAX_STATUS, "%s",
				"4 = two lines"); break;
			case 5: snprintf (sStatus, MAX_STATUS, "%s",
				"5 = one line"); break;
		}
	}

	if (strcmp (sStatus, sStatusOld) != 0) { ShowEXE(); }
}
/*****************************************************************************/
int AlignX (int iX, int iOverSixteen)
/*****************************************************************************/
{
	int iRemainder;
	int iReturn;

	/*** iQuotient = iX / 16; ***/
	iRemainder = iX % 16;
	if (iRemainder == iOverSixteen)
	{
		iReturn = iX;
	} else {
		if (iOverSixteen > iRemainder)
		{
			if (iOverSixteen - iRemainder >= 8)
			{
				iReturn = iX - ((iRemainder + 16) - iOverSixteen);
			} else {
				iReturn = iX + (iOverSixteen - iRemainder);
			}
		} else {
			if (iRemainder - iOverSixteen >= 8)
			{
				iReturn = iX + ((iOverSixteen + 16) - iRemainder);
			} else {
				iReturn = iX - (iRemainder - iOverSixteen);
			}
		}
	}
	return (iReturn);
}
/*****************************************************************************/
int AlignY (int iY, int iOverTwentyFour)
/*****************************************************************************/
{
	int iRemainder;
	int iReturn;

	/*** iQuotient = iY / 24; ***/
	iRemainder = iY % 24;
	if (iRemainder == iOverTwentyFour)
	{
		iReturn = iY;
	} else {
		if (iOverTwentyFour > iRemainder)
		{
			if (iOverTwentyFour - iRemainder >= 12)
			{
				iReturn = iY - ((iRemainder + 24) - iOverTwentyFour);
			} else {
				iReturn = iY + (iOverTwentyFour - iRemainder);
			}
		} else {
			if (iRemainder - iOverTwentyFour >= 12)
			{
				iReturn = iY + ((iOverTwentyFour + 24) - iRemainder);
			} else {
				iReturn = iY - (iRemainder - iOverTwentyFour);
			}
		}
	}
	return (iReturn);
}
/*****************************************************************************/
void DelWhat (int iType, int iX, int iY)
/*****************************************************************************/
{
	int iLoopE;
	int iXE, iYE;

	/*** Used for looping. ***/
	int iLoop;

	switch (iType)
	{
		case 1: iLoopE = ulNrFront; break;
		case 2: iLoopE = ulNrChompers; break;
		case 3: iLoopE = ulNrSpikes; break;
		case 4: iLoopE = ulNrGates; break;
		case 5: iLoopE = ulNrRaise; break;
		case 6: iLoopE = ulNrGuards; break;
		case 7: iLoopE = ulNrPotions; break;
		case 8: iLoopE = ulNrLoose; break;
		default:
			snprintf (sError, MAX_ERROR, "Unknown type: %i", iType);
			ErrorAndExit(); break;
	}
	for (iLoop = 1; iLoop <= iLoopE; iLoop++)
	{
		switch (iType)
		{
			case 1: iXE = ulFrontX[iLoop]; iYE = ulFrontY[iLoop]; break;
			case 2: iXE = ulChomperX[iLoop]; iYE = ulChomperY[iLoop]; break;
			case 3: iXE = ulSpikeX[iLoop]; iYE = ulSpikeY[iLoop]; break;
			case 4: iXE = ulGateX[iLoop]; iYE = ulGateY[iLoop]; break;
			case 5: iXE = ulRaiseX[iLoop]; iYE = ulRaiseY[iLoop]; break;
			case 6: iXE = ulGuardX[iLoop]; iYE = ulGuardY[iLoop]; break;
			case 7: iXE = ulPotionX[iLoop]; iYE = ulPotionY[iLoop]; break;
			case 8: iXE = ulLooseX[iLoop]; iYE = ulLooseY[iLoop]; break;
			default:
				snprintf (sError, MAX_ERROR, "Unknown type: %i", iType);
				ErrorAndExit(); break;
		}

		if (((abs (iX - iXE)) +
			(abs (iY - iYE))) < iXYNear)
		{
			iXYNear = (abs (iX - iXE)) +
				(abs (iY - iYE));
			iDelWhat = iType;
			iDelX = iXE;
			iDelY = iYE;
			iDelXPos = iXPos;
			iDelYPos = iYPos;
		}
	}
}
/*****************************************************************************/
void Del (void)
/*****************************************************************************/
{
	int iMove;

	/*** Used for looping. ***/
	int iLoop;

	iMove = 0;
	switch (iDelWhat)
	{
		case 1: /*** front ***/
			for (iLoop = 1; iLoop <= (int)ulNrFront; iLoop++)
			{
				if (((int)ulFrontX[iLoop] == iDelX) &&
					((int)ulFrontY[iLoop] == iDelY)) { iMove = 1; }
				if ((iMove == 1) && (iLoop != (int)ulNrFront))
				{
					ulFrontX[iLoop] = ulFrontX[iLoop + 1];
					ulFrontY[iLoop] = ulFrontY[iLoop + 1];
					ulFrontType[iLoop] = ulFrontType[iLoop + 1];
					ulFrontA[iLoop] = ulFrontA[iLoop + 1];
					ulFrontB[iLoop] = ulFrontB[iLoop + 1];
				}
			}
			ulNrFront--;
			break;
		case 2: /*** chompers ***/
			for (iLoop = 1; iLoop <= (int)ulNrChompers; iLoop++)
			{
				if (((int)ulChomperX[iLoop] == iDelX) &&
					((int)ulChomperY[iLoop] == iDelY)) { iMove = 1; }
				if ((iMove == 1) && (iLoop != (int)ulNrChompers))
				{
					ulChomperX[iLoop] = ulChomperX[iLoop + 1];
					ulChomperY[iLoop] = ulChomperY[iLoop + 1];
					ulChomperA[iLoop] = ulChomperA[iLoop + 1];
				}
			}
			ulNrChompers--;
			break;
		case 3: /*** spikes ***/
			for (iLoop = 1; iLoop <= (int)ulNrSpikes; iLoop++)
			{
				if (((int)ulSpikeX[iLoop] == iDelX) &&
					((int)ulSpikeY[iLoop] == iDelY)) { iMove = 1; }
				if ((iMove == 1) && (iLoop != (int)ulNrSpikes))
				{
					ulSpikeX[iLoop] = ulSpikeX[iLoop + 1];
					ulSpikeY[iLoop] = ulSpikeY[iLoop + 1];
					ulSpikeRight[iLoop] = ulSpikeRight[iLoop + 1];
				}
			}
			ulNrSpikes--;
			break;
		case 4: /*** gates ***/
			for (iLoop = 1; iLoop <= (int)ulNrGates; iLoop++)
			{
				if (((int)ulGateX[iLoop] == iDelX) &&
					((int)ulGateY[iLoop] == iDelY)) { iMove = 1; }
				if ((iMove == 1) && (iLoop != (int)ulNrGates))
				{
					ulGateX[iLoop] = ulGateX[iLoop + 1];
					ulGateY[iLoop] = ulGateY[iLoop + 1];
					ulGateTimeOpen[iLoop] = ulGateTimeOpen[iLoop + 1];
				}
			}
			ulNrGates--;
			break;
		case 5: /*** raise ***/
			for (iLoop = 1; iLoop <= (int)ulNrRaise; iLoop++)
			{
				if (((int)ulRaiseX[iLoop] == iDelX) &&
					((int)ulRaiseY[iLoop] == iDelY)) { iMove = 1; }
				if ((iMove == 1) && (iLoop != (int)ulNrRaise))
				{
					ulRaiseGate[iLoop] = ulRaiseGate[iLoop + 1];
					ulRaiseX[iLoop] = ulRaiseX[iLoop + 1];
					ulRaiseY[iLoop] = ulRaiseY[iLoop + 1];
				}
			}
			ulNrRaise--;
			break;
		case 6: /*** guards ***/
			for (iLoop = 1; iLoop <= (int)ulNrGuards; iLoop++)
			{
				if (((int)ulGuardX[iLoop] == iDelX) &&
					((int)ulGuardY[iLoop] == iDelY)) { iMove = 1; }
				if ((iMove == 1) && (iLoop != (int)ulNrGuards))
				{
					ulGuardX[iLoop] = ulGuardX[iLoop + 1];
					ulGuardY[iLoop] = ulGuardY[iLoop + 1];
					ulGuardDir[iLoop] = ulGuardDir[iLoop + 1];
					ulGuardHP[iLoop] = ulGuardHP[iLoop + 1];
					ulGuardA[iLoop] = ulGuardA[iLoop + 1];
					ulGuardB[iLoop] = ulGuardB[iLoop + 1];
					ulGuardC[iLoop] = ulGuardC[iLoop + 1];
					ulGuardD[iLoop] = ulGuardD[iLoop + 1];
					ulGuardE[iLoop] = ulGuardE[iLoop + 1];
					ulGuardF[iLoop] = ulGuardF[iLoop + 1];
				}
			}
			ulNrGuards--;
			break;
		case 7: /*** potions ***/
			for (iLoop = 1; iLoop <= (int)ulNrPotions; iLoop++)
			{
				if (((int)ulPotionX[iLoop] == iDelX) &&
					((int)ulPotionY[iLoop] == iDelY)) { iMove = 1; }
				if ((iMove == 1) && (iLoop != (int)ulNrPotions))
				{
					ulPotionType[iLoop] = ulPotionType[iLoop + 1];
					ulPotionX[iLoop] = ulPotionX[iLoop + 1];
					ulPotionY[iLoop] = ulPotionY[iLoop + 1];
				}
			}
			ulNrPotions--;
			break;
		case 8: /*** loose ***/
			for (iLoop = 1; iLoop <= (int)ulNrLoose; iLoop++)
			{
				if (((int)ulLooseX[iLoop] == iDelX) &&
					((int)ulLooseY[iLoop] == iDelY)) { iMove = 1; }
				if ((iMove == 1) && (iLoop != (int)ulNrLoose))
				{
					ulLooseX[iLoop] = ulLooseX[iLoop + 1];
					ulLooseY[iLoop] = ulLooseY[iLoop + 1];
					ulLooseRight[iLoop] = ulLooseRight[iLoop + 1];
				}
			}
			ulNrLoose--;
			break;
	}

	PlaySound ("wav/hum_adj.wav");
	iChanged++;
}
/*****************************************************************************/
void CreateMeta (char *sFile)
/*****************************************************************************/
{
	char sLocation[MAX_PATHFILE + 2];
	int iFd;
	char sToWrite[MAX_TOWRITE + 2];

	/*** Used for looping. ***/
	int iLoopMeta;

	/*** directory ***/
	snprintf (sLocation, MAX_PATHFILE, "%s%sMETA-INF", DIR_UNCOMP, SLASH);
	CreateDir (sLocation);

	/*** file ***/
	iFd = open (sFile, O_WRONLY|O_TRUNC|O_CREAT|O_BINARY, 0600);
	for (iLoopMeta = 0; iLoopMeta < 9; iLoopMeta++)
	{
		snprintf (sToWrite, MAX_TOWRITE, "%s\n", sMeta[iLoopMeta]);
		write (iFd, sToWrite, strlen (sToWrite));
	}
	close (iFd);
}
/*****************************************************************************/
void VerifyVersion (void)
/*****************************************************************************/
{
	char sLocation[MAX_PATHFILE + 2];

	snprintf (sLocation, MAX_PATHFILE, "%s%s0.lvl", DIR_UNCOMP, SLASH);
	if (access (sLocation, R_OK) == -1)
	{
		snprintf (sError, MAX_ERROR, "Your JAR is not a 176x208 version without"
			" Nokia UI API! Delete %s and %s%s.", sPathFile, DIR_UNCOMP, SLASH);
		ErrorAndExit();
	}
}
/*****************************************************************************/
void InitPopUp (void)
/*****************************************************************************/
{
	int iPopUp;
	SDL_Event event;

	iPopUp = 1;

	PlaySound ("wav/popup.wav");
	ShowPopUp();
	while (iPopUp == 1)
	{
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_CONTROLLERBUTTONDOWN:
					/*** Nothing for now. ***/
					break;
				case SDL_CONTROLLERBUTTONUP:
					switch (event.cbutton.button)
					{
						case SDL_CONTROLLER_BUTTON_A:
							iPopUp = 0; break;
					}
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
						case SDLK_KP_ENTER:
						case SDLK_RETURN:
						case SDLK_SPACE:
						case SDLK_o:
							iPopUp = 0;
							break;
					}
					break;
				case SDL_MOUSEMOTION:
					iXPos = event.motion.x;
					iYPos = event.motion.y;
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 1)
					{
						if (InArea (450, 430, 450 + 85, 430 + 32) == 1) /*** OK ***/
							{ iOKOn = 1; }
					}
					ShowPopUp();
					break;
				case SDL_MOUSEBUTTONUP:
					iOKOn = 0;
					if (event.button.button == 1)
					{
						if (InArea (450, 430, 450 + 85, 430 + 32) == 1) /*** OK ***/
							{ iPopUp = 0; }
					}
					ShowPopUp();
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_EXPOSED)
						{ ShowScreen(); ShowPopUp(); } break;
				case SDL_QUIT:
					Quit(); break;
			}
		}

		/*** prevent CPU eating ***/
		gamespeed = REFRESH_PROG;
		while ((SDL_GetTicks() - looptime) < gamespeed)
		{
			SDL_Delay (10);
		}
		looptime = SDL_GetTicks();
	}
	PlaySound ("wav/popup_close.wav");
	ShowScreen();
}
/*****************************************************************************/
void ShowPopUp (void)
/*****************************************************************************/
{
	/*** faded background ***/
	ShowImage (imgfaded, 0, 0, "imgfaded", ascreen, iScale, 1);

	/*** popup ***/
	ShowImage (imgpopup, 109, 118, "imgpopup", ascreen, iScale, 1);

	/*** OK ***/
	switch (iOKOn)
	{
		case 0: ShowImage (imgok[1], 450, 430, "imgok[1]",
			ascreen, iScale, 1); break; /*** up ***/
		case 1: ShowImage (imgok[2], 450, 430, "imgok[2]",
			ascreen, iScale, 1); break; /*** down ***/
	}

	snprintf (arText[0], MAX_TEXT, "%s %s", EDITOR_NAME, EDITOR_VERSION);
	snprintf (arText[1], MAX_TEXT, "%s", COPYRIGHT);
	snprintf (arText[2], MAX_TEXT, "%s", "");
	if (iController != 1)
	{
		snprintf (arText[3], MAX_TEXT, "%s",
			"This program is free software; GPL3+.");
		snprintf (arText[4], MAX_TEXT, "%s", "gnu.org/licenses/gpl-3.0.en.html");
		snprintf (arText[5], MAX_TEXT, "%s", "Perseverance is prince.");
	} else {
		snprintf (arText[3], MAX_TEXT, "%s", "The detected game controller:");
		snprintf (arText[4], MAX_TEXT, "%s", sControllerName);
		snprintf (arText[5], MAX_TEXT, "%s", "Have fun using it!");
	}

	DisplayText (190, 282, 15, 6, color_wh, 1);

	/*** refresh screen ***/
	SDL_RenderPresent (ascreen);
}
/*****************************************************************************/
void UpdateHover (void)
/*****************************************************************************/
{
	int iX, iY;
	int iXFull, iYFull;

	/*** Used for looping. ***/
	int iRowLoop, iColumnLoop;

	iY = 0;
	for (iRowLoop = 1; iRowLoop <= (int)ulHeight; iRowLoop++)
	{
		iX = 0;
		for (iColumnLoop = 1; iColumnLoop <= (int)ulWidth; iColumnLoop++)
		{
			iXFull = MapStartX() + (iX * iZoom);
			iYFull = MapStartY() + (iY * iZoom);
			if ((iXPos >= iXFull) && (iXPos < (iXFull + (16 * iZoom))) &&
				(iYPos >= iYFull) && (iYPos < (iYFull + (24 * iZoom))))
			{
				iHoverRow = iRowLoop;
				iHoverColumn = iColumnLoop;
			}
			iX+=16;
		}
		iY+=24;
	}
}
/*****************************************************************************/
