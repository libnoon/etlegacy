/*
 * Wolfenstein: Enemy Territory GPL Source Code
 * Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.
 *
 * ET: Legacy
 * Copyright (C) 2012-2016 ET:Legacy team <mail@etlegacy.com>
 *
 * This file is part of ET: Legacy - http://www.etlegacy.com
 *
 * ET: Legacy is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ET: Legacy is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ET: Legacy. If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, Wolfenstein: Enemy Territory GPL Source Code is also
 * subject to certain additional terms. You should have received a copy
 * of these additional terms immediately following the terms and conditions
 * of the GNU General Public License which accompanied the source code.
 * If not, please request a copy in writing from id Software at the address below.
 *
 * id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.
 */
/**
 * @file sys_main.c
 */

#include <signal.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "sys_local.h"
#include "sys_loadlib.h"

#include "../qcommon/q_shared.h"
#include "../qcommon/qcommon.h"

#ifdef _WIN32
#include <windows.h>
#include "sys_win32.h"
#endif

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

static char binaryPath[MAX_OSPATH] = { 0 };
static char installPath[MAX_OSPATH] = { 0 };

#ifdef FEATURE_CURSES
static qboolean nocurses = qfalse;
void CON_Init_tty(void);
#endif

/*
=================
Sys_SetBinaryPath
=================
*/
void Sys_SetBinaryPath(const char *path)
{
	Q_strncpyz(binaryPath, path, sizeof(binaryPath));
}

/*
=================
Sys_BinaryPath
=================
*/
char *Sys_BinaryPath(void)
{
	return binaryPath;
}

/*
=================
Sys_SetDefaultInstallPath
=================
*/
void Sys_SetDefaultInstallPath(const char *path)
{
	Q_strncpyz(installPath, path, sizeof(installPath));
}

/*
=================
Sys_DefaultInstallPath
=================
*/
char *Sys_DefaultInstallPath(void)
{
	if (*installPath)
	{
		return installPath;
	}
	else
	{
		return Sys_Cwd();
	}
}

/**
 * @brief Restart the input subsystem
 */
void Sys_In_Restart_f(void)
{
	IN_Restart();
}

/**
 * @brief Handle new console input
 */
#ifndef USE_WINDOWS_CONSOLE
char *Sys_ConsoleInput(void)
{
	return CON_Input();
}
#endif

/**
 * @brief Writes pid to profile or to the homepath root if running a server
 * @retval qtrue  if pid file successfully created
 * @retval qfalse if it wasn't possible to create a new pid file
 */
qboolean Sys_WritePIDFile(void)
{
	fileHandle_t f;

	// First, check if the pid file is already there
	if (FS_FileInPathExists(com_pidfile->string))
	{
		// TODO: check if we are hijacking live pid file
		/*
		FS_FOpenFileRead(com_pidfile->string, &f, qtrue);

		if(Sys_PIDIsRunning(pid))
		{
		    Com_Printf("WARNING: another instance of ET:L is using this path!\n");
		    return qfalse;
		}
		*/
		if (FS_Delete(com_pidfile->string) == 0) // stale pid from previous run
		{
			Com_Printf("WARNING: unable to delete old PID file!\n");
		}
	}

	f = FS_FOpenFileWrite(com_pidfile->string);
	if (f < 0)
	{
		Com_Printf("WARNING: can't write PID file!\n");
		return qfalse;
	}
	else
	{
		Com_Printf("Creating PID file '%s'\n", com_pidfile->string);
	}

	FS_Printf(f, "%d", com_pid->integer);

	FS_FCloseFile(f);

	// track profile changes
	Com_TrackProfile(com_pidfile->string);

	return qtrue;
}

/**
 * @brief Single exit point (regular exit or in case of error)
 */
static __attribute__ ((noreturn)) void Sys_Exit(int exitCode)
{
	CON_Shutdown();

#ifndef DEDICATED
	SDL_Quit();
#endif

	// fail safe: delete PID file on abnormal exit
	// FIXME: normal exit pid deletion is done in Com_Shutdown
	//        why do we have 2 locations for this job?
	//        ... is this Com or Sys code?
	if (exitCode > 0)
	{
		// Normal exit
		// com_pidfile does not yet exist on early exit
		if (Cvar_VariableString("com_pidfile")[0] != '\0')
		{
			if (FS_FileExists(Cvar_VariableString("com_pidfile")))
			{
				// FIXME: delete even when outside of homepath
				if (remove(va("%s%c%s%c%s", Cvar_VariableString("fs_homepath"),
				              PATH_SEP, Cvar_VariableString("fs_game"),
				              PATH_SEP, Cvar_VariableString("com_pidfile"))) != 0)
				{
					// This is thrown when game game crashes before PID file is created
					// f.e. when pak files are missing
					// FIXME: try to create PID file earlier
					Com_Printf("Sys_Exit warning - can't delete PID file %s%c%s%c%s\n", Cvar_VariableString("fs_homepath"),
					           PATH_SEP, Cvar_VariableString("fs_game"),
					           PATH_SEP, Cvar_VariableString("com_pidfile"));
				}
				else
				{
					Com_Printf("PID file removed.\n");
				}
			}
			else
			{
				Com_Printf("Sys_Exit warning - PID file doesn't exist %s%c%s%c%s\n", Cvar_VariableString("fs_homepath"),
				           PATH_SEP, Cvar_VariableString("fs_game"),
				           PATH_SEP, Cvar_VariableString("com_pidfile"));
			}
		}
		else
		{
			Com_Printf("Sys_Exit warning no PID file found to remove\n");
		}
	}

	NET_Shutdown();

	exit(exitCode);
}


/*
=================
Sys_Quit
=================
*/
void Sys_Quit(void)
{
	Sys_Exit(0);
#ifdef USE_WINDOWS_CONSOLE
	Sys_DestroyConsole();
#endif
}

/*
=================
Sys_Init
=================
*/
#ifdef USE_WINDOWS_CONSOLE
extern void Sys_ClearViewlog_f(void);
#endif

void Sys_Init(void)
{
	Cmd_AddCommand("in_restart", Sys_In_Restart_f);
#ifdef USE_WINDOWS_CONSOLE
	Cmd_AddCommand("clearviewlog", Sys_ClearViewlog_f);
#endif

	Cvar_Set("arch", OS_STRING " " ARCH_STRING);
	Cvar_Set("username", Sys_GetCurrentUser());
}

/**
 * @brief Transform Q3 colour codes to ANSI escape sequences
 */
void Sys_AnsiColorPrint(const char *msg)
{
	static char buffer[MAXPRINTMSG];
	int         i, j, _found, length = 0;

	// colors hash from http://wolfwiki.anime.net/index.php/Color_Codes
	static int etAnsiHash[][6] =
	{
		// here should be black, but it's invisible in terminal
		// so you see dark grey here. like in 9 color
		{ 1,  30, 0,   '0', 'p',  'P'  },
		{ 1,  31, 0,   '1', 'q',  'Q'  },
		{ 1,  32, 0,   '2', 'r',  'R'  },
		{ 1,  33, 0,   '3', 's',  'S'  },
		{ 1,  34, 0,   '4', 't',  'T'  },
		{ 1,  36, 0,   '5', 'u',  'U'  },
		{ 1,  35, 0,   '6', 'v',  'V'  },
		{ 1,  37, 0,   '7', 'w',  'W'  },

		{ 38, 5,  208, '8', 'x',  'X'  },
		{ 1,  30, 0,   '9', 'y',  'Y'  },
		{ 0,  37, 0,   'z', 'Z',  ':'  }, // the same
		{ 0,  37, 0,   '[', '{',  ';'  }, // --------
		{ 0,  32, 0,   '<', '\\', '|'  },
		{ 0,  33, 0,   '=', ']',  '}'  },
		{ 0,  34, 0,   '>', '~',  '^'  },
		{ 0,  31, 0,   '?', '_',  0    },
		{ 38, 5,  94,  '@', '`',  0    },

		{ 38, 5,  214, 'a', 'A',  '!'  },
		{ 38, 5,  30,  'b', 'B',  '"'  },
		{ 38, 5,  90,  'c', 'C',  '#'  },
		{ 38, 5,  33,  'd', 'D',  '$'  },
		{ 38, 5,  93,  'e', 'E',  '%'  },
		{ 38, 5,  38,  'f', 'F',  '&'  },
		{ 38, 5,  194, 'g', 'G',  '\'' },
		{ 38, 5,  29,  'h', 'H',  '('  },

		{ 38, 5,  197, 'i', 'I',  ')'  },
		{ 38, 5,  124, 'j', 'J',  '*'  },
		{ 38, 5,  130, 'k', 'K',  '+'  },
		{ 38, 5,  179, 'l', 'L',  ','  },
		{ 38, 5,  143, 'm', 'M',  '-'  },
		{ 38, 5,  229, 'n', 'N',  '.'  },
		{ 38, 5,  228, 'o', 'O',  '/'  }
	};

	while (*msg)
	{
		if (Q_IsColorString(msg) || *msg == '\n' || *msg == '\\')
		{
			// First empty the buffer
			if (length > 0)
			{
				buffer[length] = '\0';
				fputs(buffer, stderr);
				length = 0;
			}

			if (*msg == '\n')
			{
				// Issue a reset and then the newline
				fputs("\033[0m\n", stderr);
				msg++;
			}
			else if (*msg == '\\')
			{
				fputs("\033[0m\\", stderr);
				msg++;
			}
			else if (*(msg + 1) == '7' && (*(msg + 2) == '"' || *(msg + 2) == '\'' || *(msg + 2) == '\0'))
			{
				fputs("\033[0m", stderr);
				msg += 2;
			}
			else
			{
				// Print the color code
				_found = 0;
				for (i = 0; i < 32; i++)
				{
					if (_found)
					{
						break;
					}

					for (j = 3; j < 6; j++)
					{
						if (etAnsiHash[i][j] == 0)
						{
							break;
						}

						if (*(msg + 1) == etAnsiHash[i][j])
						{
							if (etAnsiHash[i][2] == 0)
							{
								Com_sprintf(buffer, sizeof(buffer), "\033[%d;%dm",
								            etAnsiHash[i][0], etAnsiHash[i][1]);
								_found = 1;
								break;
							}
							else
							{
								Com_sprintf(buffer, sizeof(buffer), "\033[%d;%d;%dm",
								            etAnsiHash[i][0], etAnsiHash[i][1], etAnsiHash[i][2]);
								_found = 1;
								break;
							}
						}
					}
				}

				fputs(buffer, stderr);
				msg += 2;
			}
		}
		else
		{
			if (length >= MAXPRINTMSG - 1)
			{
				break;
			}

			buffer[length] = *msg;
			length++;
			msg++;
		}
	}

	// Empty anything still left in the buffer
	if (length > 0)
	{
		buffer[length] = '\0';
		fputs(buffer, stderr);
	}
}

/*
=================
Sys_Print
=================
*/
void Sys_Print(const char *msg)
{
#ifdef USE_WINDOWS_CONSOLE
	Conbuf_AppendText(msg);
#else
	CON_LogWrite(msg);
	CON_Print(msg);
#endif

#if defined(LEGACY_DEBUG) && defined(_WIN32)
	OutputDebugString(msg);
#endif
}

/*
=================
Sys_Error
=================
*/
void Sys_Error(const char *error, ...)
{
	va_list argptr;
	char    string[1024];
#ifdef USE_WINDOWS_CONSOLE
	MSG msg;
#endif

	va_start(argptr, error);
	Q_vsnprintf(string, sizeof(string), error, argptr);
	va_end(argptr);

#ifdef _WIN32
	Sys_Splash(qfalse);
#endif

#ifdef USE_WINDOWS_CONSOLE
	Conbuf_AppendText(string);
	Conbuf_AppendText("\n");

	Sys_SetErrorText(string);
	Sys_ShowConsoleWindow(1, qtrue);

	IN_Shutdown();

	// wait for the user to quit
	while (1)
	{
		if (!GetMessage(&msg, NULL, 0, 0))
		{
			Com_Quit_f();
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Sys_DestroyConsole();
#endif

	Sys_ErrorDialog(string);

	Sys_Exit(3);
}

/*
============
Sys_FileTime

returns -1 if not present
============
*/
int Sys_FileTime(char *path)
{
	struct stat buf;

	if (stat(path, &buf) == -1)
	{
		return -1;
	}

	return buf.st_mtime;
}

/*
=================
Sys_UnloadDll
=================
*/
void Sys_UnloadDll(void *dllHandle)
{
	if (!dllHandle)
	{
		Com_Printf("Sys_UnloadDll(NULL)\n");
		return;
	}

	Sys_UnloadLibrary(dllHandle);
}

/*
=================
Sys_LoadDll

First try to load library name from system library path,
from executable path, then fs_basepath.
=================
*/

void *Sys_LoadDll(const char *name, qboolean useSystemLib)
{
	void *dllhandle;

	if (useSystemLib)
	{
		Com_Printf("Trying to load \"%s\"...\n", name);
	}

	if (!useSystemLib || !(dllhandle = Sys_LoadLibrary(name)))
	{
		const char *topDir;
		char       libPath[MAX_OSPATH];

		topDir = Sys_BinaryPath();

		if (!*topDir)
		{
			topDir = ".";
		}

		Com_Printf("Trying to load \"%s\" from \"%s\"...\n", name, topDir);
		Com_sprintf(libPath, sizeof(libPath), "%s%c%s", topDir, PATH_SEP, name);

		if (!(dllhandle = Sys_LoadLibrary(libPath)))
		{
			const char *basePath = Cvar_VariableString("fs_basepath");

			if (!basePath || !*basePath)
			{
				basePath = ".";
			}

			if (FS_FilenameCompare(topDir, basePath))
			{
				Com_Printf("Trying to load \"%s\" from \"%s\"...\n", name, basePath);
				Com_sprintf(libPath, sizeof(libPath), "%s%c%s", basePath, PATH_SEP, name);
				dllhandle = Sys_LoadLibrary(libPath);
			}

			if (!dllhandle)
			{
				Com_Printf("Loading \"%s\" failed\n", name);
			}
		}
	}

	return dllhandle;
}

/**
 * @brief Used by Sys_LoadGameDll to get handle on a mod library
 * @return Handle to a mod library
 */
static void *Sys_TryLibraryLoad(const char *base, const char *gamedir, const char *fname)
{
	void *libHandle;
	char *fn;

#ifdef __APPLE__
	Com_Printf("Sys_LoadDll -> Sys_TryLibraryLoad(%s, %s, %s)... \n", base, gamedir, fname);
	libHandle = NULL;

	// Incoming is (for example) "cgame_mac"
	// What we may actually have is:
	// 1) A zipped .bundle package
	// 2) A .dylib without an extension
	// 3) A .dylib with an extension we need to append.
	//
	// In older versions of OS X (pre 10.5), dylibs could not be unloaded allowing the host process to reclaim that address space.
	// This is why .bundles were used instead. When W:ET originally shipped, it used used .bundle packages for the VM libraries,
	// but to make them single files, it zipped that .bundle package and had no extension at all (ie just "cgame_mac"). But now
	// that dylibs can be unloaded, there's no practical difference between the two (for W:ET's purposes), so using a single file
	// dylib is simpler. That's why we now support a dylib with some backward compatibility to allow .bundles.

	// 1: The zipped .bundle package
	{
		Com_Printf("-- Trying zipped .bundle... ");
		fn = FS_BuildOSPath(base, gamedir, fname);
		if (FS_Unzip(fn, qtrue))
		{
			char buffer[MAX_OSPATH];
			Com_sprintf(buffer, sizeof(buffer), "%s.bundle/Contents/MacOS/%s", fname, fname);
			fn = FS_BuildOSPath(Cvar_VariableString("fs_homepath"), gamedir, buffer);

			libHandle = Sys_LoadLibrary(fn);
			if (!libHandle)
			{
				Com_Printf("failed: \"%s\"\n", Sys_LibraryError());
			}
			else
			{
				Com_Printf("succeeded\n");
			}
		}
		else
		{
			Com_Printf("failed. (Not a valid zip).\n");
		}
	}

	// 2: The dylib without an extension
	if (!libHandle)
	{
		fn = FS_BuildOSPath(base, gamedir, fname);

		Com_Printf("-- Trying extension-less dylib... ");
		libHandle = Sys_LoadLibrary(fn);
		if (!libHandle)
		{
			Com_Printf("failed: \"%s\"\n", Sys_LibraryError());
		}
		else
		{
			Com_Printf("succeeded\n");
		}
	}

	// 3: The dylib with an extension
	if (!libHandle)
	{
		char buffer[MAX_OSPATH];
		Com_sprintf(buffer, sizeof(buffer), "%s.dylib", fname);
		fn = FS_BuildOSPath(Cvar_VariableString("fs_homepath"), gamedir, buffer);

		Com_Printf("-- Trying dylib with extension... ");
		libHandle = Sys_LoadLibrary(fn);
		if (!libHandle)
		{
			Com_Printf("failed: \"%s\"\n", Sys_LibraryError());
		}
		else
		{
			Com_Printf("succeeded\n");
		}
	}

	return libHandle;

#else // __APPLE__

	fn = FS_BuildOSPath(base, gamedir, fname);
	Com_Printf("Sys_LoadDll(%s)... ", fn);

	libHandle = Sys_LoadLibrary(fn);

	if (!libHandle)
	{
		Com_Printf("failed: \"%s\"\n", Sys_LibraryError());
		return NULL;
	}

#endif // __APPLE__

	Com_Printf("succeeded\n");

	return libHandle;
}

/**
 * @brief Loads a mod library.
 * #1 look in fs_homepath
 * #2 look in fs_basepath
 * #3 try to revert to the default mod library
 */
void *Sys_LoadGameDll(const char *name, qboolean extract,
                      intptr_t(**entryPoint) (int, ...),
                      intptr_t (*systemcalls)(intptr_t, ...))
{
	void *libHandle;
	void (*dllEntry)(intptr_t (*syscallptr)(intptr_t, ...));
	char fname[MAX_OSPATH];
	char *basepath;
	char *homepath;
	char *gamedir;

	assert(name);

	Com_sprintf(fname, sizeof(fname), Sys_GetDLLName("%s"), name);

	// TODO: use fs_searchpaths from files.c
	basepath = Cvar_VariableString("fs_basepath");
	homepath = Cvar_VariableString("fs_homepath");
	gamedir  = Cvar_VariableString("fs_game");

	// STORY TIME
	//
	//When doing an debug build just load the mod lib from the basepath as that will have the debug pointers
	//
	// Now the code always just unpacks new libraries from the packs on release builds.
	// So the libraryfiles in the homepath are always refreshed with latest from the packs.
	// This fixes many issues with clients loading old libraries from the mod paths.
	//
	// The way it used to work is described under (or in debug mode)..
	//
	// if the server is pure, extract the dlls from the mod_bin.pk3 so
	// that they can be referenced
	//
	// If the server is not pure, then the
	// lib must either already be in the homepath, or in the basepath,
	// without being in a pak. For a pure server, it always grabs the lib
	// from within a pak. This means there must be two copies of the lib!
	//
	// So now, if connecting to an impure server, and the lib was not
	// loaded from homepath or the basepath, let's pull it out of the pak.
	// This means we only *need* the copy that's in the pak, and will use
	// it if another copy isn't found first.
#ifdef LEGACY_DEBUG
#define SEARCHPATH1 basepath
#define SEARCHPATH2 homepath
#define LIB_DO_UNPACK Cvar_VariableIntegerValue("sv_pure")
#else
#define LIB_DO_UNPACK qtrue
#define SEARCHPATH1 homepath
#define SEARCHPATH2 basepath
#endif

#ifndef DEDICATED
	if (LIB_DO_UNPACK && extract)
	{
		Com_Printf("Sys_LoadGameDll -> FS_CL_ExtractFromPakFile(%s, %s, %s)\n", homepath, gamedir, fname);
		FS_CL_ExtractFromPakFile(homepath, gamedir, fname);
	}
#endif

	libHandle = Sys_TryLibraryLoad(SEARCHPATH1, gamedir, fname);

	if (!libHandle && SEARCHPATH2)
	{
		libHandle = Sys_TryLibraryLoad(SEARCHPATH2, gamedir, fname);
	}

#ifndef DEDICATED
	if (!libHandle && !LIB_DO_UNPACK && extract)
	{
		Com_Printf("Sys_LoadGameDll -> FS_CL_ExtractFromPakFile(%s, %s, %s)\n", homepath, gamedir, fname);
		FS_CL_ExtractFromPakFile(homepath, gamedir, fname);
		libHandle = Sys_TryLibraryLoad(homepath, gamedir, fname);
	}
#endif

	if (!libHandle)
	{
		Com_Printf("Sys_LoadDll(%s/%s) failed to load library\n", gamedir, name);
		return NULL;
	}

	dllEntry    = (void (QDECL *)(intptr_t (QDECL *)(intptr_t, ...)))Sys_LoadFunction(libHandle, "dllEntry");
	*entryPoint = (intptr_t (QDECL *)(int, ...))Sys_LoadFunction(libHandle, "vmMain");

	if (!*entryPoint || !dllEntry)
	{
		Com_Printf("Sys_LoadDll(%s/%s) failed to find vmMain function:\n\"%s\" !\n", gamedir, name, Sys_LibraryError());
		Sys_UnloadLibrary(libHandle);

		return NULL;
	}

	Com_Printf("Sys_LoadDll(%s/%s) found vmMain function at %p\n", gamedir, name, *entryPoint);
	dllEntry(systemcalls);

	return libHandle;
}

/*
=================
Sys_ParseArgs
=================
*/
void Sys_ParseArgs(int argc, char **argv)
{
#ifdef FEATURE_CURSES
	int i;
#endif

	if (argc == 2)
	{
		if (!strcmp(argv[1], "--version") ||
		    !strcmp(argv[1], "-v"))
		{
#ifdef DEDICATED
			fprintf(stdout, Q3_VERSION " " CPUSTRING " dedicated server (%s)\n", __DATE__);
#else
			fprintf(stdout, "Client: " ET_VERSION "\n");
			fprintf(stdout, "Masked as: " FAKE_VERSION "\n");
#endif
			Sys_Exit(0);
		}
	}
#ifdef FEATURE_CURSES
	for (i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "+nocurses") ||
		    !strcmp(argv[i], "--nocurses"))
		{
			nocurses = qtrue;
			break;
		}
	}
#endif
}

void Sys_BuildCommandLine(int argc, char **argv, char *buffer, int bufferSize)
{
	int i = 0;
	// Concatenate the command line for passing to Com_Init
	for (i = 1; i < argc; i++)
	{
		const qboolean containsSpaces = (qboolean)(strchr(argv[i], ' ') != NULL);

		// Allow URIs to be passed without +connect
		if (!Q_strncmp(argv[i], "et://", 5) && Q_strncmp(argv[i - 1], "+connect", 8))
		{
			Q_strcat(buffer, bufferSize, "+connect ");
		}

		if (containsSpaces)
		{
			Q_strcat(buffer, bufferSize, "\"");
		}

		Q_strcat(buffer, bufferSize, argv[i]);

		if (containsSpaces)
		{
			Q_strcat(buffer, bufferSize, "\"");
		}

		Q_strcat(buffer, bufferSize, " ");
	}
}

#ifndef DEFAULT_BASEDIR
#       define DEFAULT_BASEDIR Sys_BinaryPath()
#endif

/*
=================
Sys_SigHandler
=================
*/
void Sys_SigHandler(int signal)
{
	static qboolean signalcaught = qfalse;

	if (signalcaught)
	{
		fprintf(stderr, "DOUBLE SIGNAL FAULT: Received signal %d, exiting...\n",
		        signal);
	}
	else
	{
		signalcaught = qtrue;
#ifndef DEDICATED
		CL_Shutdown();
#endif
		SV_Shutdown(va("Received signal %d", signal));
	}

	if (signal == SIGTERM || signal == SIGINT)
	{
		Sys_Exit(1);
	}
	else
	{
		Sys_Exit(2);
	}
}

void Sys_SetUpConsoleAndSignals(void)
{
#ifndef USE_WINDOWS_CONSOLE
#ifdef FEATURE_CURSES
	if (nocurses)
	{
		CON_Init_tty();
	}
	else
	{
		CON_Init();
	}
#else
	CON_Init();
#endif
#endif

	signal(SIGILL, Sys_SigHandler);
	signal(SIGFPE, Sys_SigHandler);
	signal(SIGSEGV, Sys_SigHandler);
	signal(SIGTERM, Sys_SigHandler);
	signal(SIGINT, Sys_SigHandler);
}

/*
=================
Sys_GameLoop
Main game loop
=================
*/
void Sys_GameLoop(void)
{
#ifdef LEGACY_DEBUG
	int startTime, endTime, totalMsec, countMsec;
	startTime = endTime = totalMsec = countMsec = 0;
#endif

	while (qtrue)
	{
#if defined(_MSC_VER) && defined(LEGACY_DEBUG) && !defined(_WIN64)
		// set low precision every frame, because some system calls
		// reset it arbitrarily
		_controlfp(_PC_24, _MCW_PC);
		_controlfp(-1, _MCW_EM);    // no exceptions, even if some crappy
		// syscall turns them back on!
#endif

#ifdef LEGACY_DEBUG
		startTime = Sys_Milliseconds();
#endif

		IN_Frame();
		Com_Frame();

#ifdef LEGACY_DEBUG
		endTime    = Sys_Milliseconds();
		totalMsec += endTime - startTime;
		countMsec++;

		if (com_speeds->integer)
		{
			Com_Printf("frame:%i total used:%i frame time:%i\n", countMsec, totalMsec, endTime - startTime);
		}
#endif
	}
}

/*
=================
main
=================
*/
int main(int argc, char **argv)
{
	char commandLine[MAX_STRING_CHARS] = { 0 };

	Sys_PlatformInit();

	// Set the initial time base
	Sys_Milliseconds();

#ifdef __APPLE__
	// This is passed if we are launched by double-clicking
	if (argc >= 2 && Q_strncmp(argv[1], "-psn", 4) == 0)
	{
		argc = 1;
	}
#endif

	Sys_ParseArgs(argc, argv);

#if defined(__APPLE__) && !defined(DEDICATED)
	// argv[0] would be /Users/seth/etlegacy/etl.app/Contents/MacOS
	// But on OS X we want to pretend the binary path is the .app's parent
	// So that way the base folder is right next to the .app allowing
	{
		char     parentdir[1024];
		CFURLRef url = CFBundleCopyBundleURL(CFBundleGetMainBundle());
		if (!url)
		{
			Sys_Dialog(DT_ERROR, "A CFURL for the app bundle could not be found.", "Can't set Sys_SetBinaryPath");
			Sys_Exit(1);
		}

		CFURLRef url2 = CFURLCreateCopyDeletingLastPathComponent(0, url);
		if (!url2 || !CFURLGetFileSystemRepresentation(url2, 1, (UInt8 *)parentdir, 1024))
		{
			Sys_Dialog(DT_ERROR, "CFURLGetFileSystemRepresentation returned an error when finding the app bundle's parent directory.", "Can't set Sys_SetBinaryPath");
			Sys_Exit(1);
		}

		Sys_SetBinaryPath(parentdir);

		CFRelease(url);
		CFRelease(url2);
	}
#else
	Sys_SetBinaryPath(Sys_Dirname(argv[0]));
#endif

	Sys_SetDefaultInstallPath(DEFAULT_BASEDIR); // Sys_BinaryPath() by default

	// Concatenate the command line for passing to Com_Init
	Sys_BuildCommandLine(argc, argv, commandLine, sizeof(commandLine));

	Com_Init(commandLine);
	NET_Init();

	Sys_SetUpConsoleAndSignals();

#ifdef _WIN32

#ifndef DEDICATED
	if(com_viewlog->integer)
	{
		Sys_ShowConsoleWindow(1, qfalse);
	}
#endif

	Sys_Splash(qfalse);

	{
		char cwd[MAX_OSPATH];
		_getcwd(cwd, sizeof(cwd));
		Com_Printf("Working directory: %s\n", cwd);
	}

	// hide the early console since we've reached the point where we
	// have a working graphics subsystems
#ifndef LEGACY_DEBUG
	if (!com_dedicated->integer && !com_viewlog->integer)
	{
		Sys_ShowConsoleWindow(0, qfalse);
	}
#endif

#endif

	Sys_GameLoop();

	return 0;
}