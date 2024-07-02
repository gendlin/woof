//
//  Copyright (C) 1999 by
//  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
// DESCRIPTION:
//      Main program, simply calls D_DoomMain high level loop.
//
//-----------------------------------------------------------------------------

#include "SDL.h"

#include <stdlib.h>
#include <locale.h>

#include "config.h"
#include "i_printf.h"
#include "i_system.h"
#include "m_argv.h"
#include "version.h"
#include "i_timer.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

static uint64_t count = 1000000000;

void PrintResult(const wchar_t* name, uint32_t milliseconds)
{
    double nsPerCall = (double)milliseconds * 1e6 / (double)count;
    uint32_t seconds = milliseconds / 1000; milliseconds %= 1000;
    uint32_t minutes = seconds / 60; seconds %= 60;
    uint32_t hours = minutes / 60; minutes %= 60;
    wprintf(L"According to %s it took %u:%02u:%02u.%03u (%g ns per call)\n", name, hours, minutes, seconds, milliseconds, nsPerCall);
}

int qpctest()
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);

    //if( argc == 2 )
    //    swscanf_s(argv[1], L"%zu", &count);
    {
        wprintf(L"Executing QueryPerformanceCounter x %zu...\n", count);

        LARGE_INTEGER begPerfCount;
        LARGE_INTEGER lastPerfCount;
        //LARGE_INTEGER sumPerfCount{}; // Unused.

        uint64_t begTickCount = GetTickCount64();
        for( size_t i = 0; i < count; ++i )
        {
            QueryPerformanceCounter(&lastPerfCount);
            //sumPerfCount.QuadPart += lastPerfCount.QuadPart;
            if( i == 0 )
                begPerfCount.QuadPart = lastPerfCount.QuadPart;
        }
        uint64_t endTickCount = GetTickCount64();

        PrintResult(L"GetTickCount64", (uint32_t)(endTickCount - begTickCount));

        uint32_t qpcMilliseconds = (uint32_t)(uint64_t)(((lastPerfCount.QuadPart - begPerfCount.QuadPart) * 1000. / freq.QuadPart) + 0.5);
        PrintResult(L"QueryPerformanceCounter", qpcMilliseconds);
    }

    {
        wprintf(L"\nExecuting SDL_GetPerformanceCounter x %zu...\n", count);
        uint64_t first_time = 0;
        uint64_t last_time = 0;

        uint64_t begTickCount = GetTickCount64();
        for( size_t i = 0; i < count; ++i )
        {
            last_time = SDL_GetPerformanceCounter();
            if( i == 0 )
                first_time = last_time;
        }
        uint64_t endTickCount = GetTickCount64();

        PrintResult(L"GetTickCount64", (uint32_t)(endTickCount - begTickCount));

        PrintResult(L"SDL_GetPerformanceCounter", (last_time - first_time) * 1000 / SDL_GetPerformanceFrequency());
    }

    {
        wprintf(L"\nExecuting (double) I_GetTimeUS x %zu...\n", count);
        double first_time = 0.;
        double last_time = 0.;

        uint64_t begTickCount = GetTickCount64();
        for( size_t i = 0; i < count; ++i )
        {
            last_time = I_GetTimeUS();
            if( i == 0 )
                first_time = last_time;
        }
        uint64_t endTickCount = GetTickCount64();

        PrintResult(L"GetTickCount64", (uint32_t)(endTickCount - begTickCount));

        PrintResult(L"(double) I_GetTimeUS", (last_time - first_time) * 1e-3);
    }

    {
        wprintf(L"\nExecuting (uint64) I_GetTimeUS x %zu...\n", count);
        uint64_t first_time = 0;
        uint64_t last_time = 0;

        uint64_t begTickCount = GetTickCount64();
        for( size_t i = 0; i < count; ++i )
        {
            last_time = I_GetTimeUSold();
            if( i == 0 )
                first_time = last_time;
        }
        uint64_t endTickCount = GetTickCount64();

        PrintResult(L"GetTickCount64", (uint32_t)(endTickCount - begTickCount));

        PrintResult(L"(uint64) I_GetTimeUS", (last_time - first_time) / 1000);
    }

    return 0;
}


//
// D_DoomMain()
// Not a globally visible function, just included for source reference,
// calls all startup code, parses command line options.
//

void D_DoomMain(void);

#if defined(WIN_LAUNCHER)
__declspec(dllexport) void Woof_Exit(void)
{
    I_SafeExit(0);
}
#endif

#if defined(WIN_LAUNCHER)
__declspec(dllexport) int Woof_Main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
   myargc = argc;
   myargv = argv;

   // Print date and time in the Load/Save Game menus in the current locale
   setlocale(LC_TIME, "");

   I_Printf(VB_ALWAYS, "%s (built on %s)\n", PROJECT_STRING, version_date);

   //!
   //
   // Print the program version and exit.
   //

   if (M_ParmExists("-version") || M_ParmExists("--version"))
   {
      exit(0);
   }

   I_InitTimer();

   qpctest();

   //D_DoomMain();

   return 0;
}

//----------------------------------------------------------------------------
//
// $Log: i_main.c,v $
// Revision 1.8  1998/05/15  00:34:03  killough
// Remove unnecessary crash hack
//
// Revision 1.7  1998/05/13  22:58:04  killough
// Restore Doom bug compatibility for demos
//
// Revision 1.6  1998/05/03  22:38:36  killough
// beautification
//
// Revision 1.5  1998/04/27  02:03:11  killough
// Improve signal handling, to use Z_DumpHistory()
//
// Revision 1.4  1998/03/09  07:10:47  killough
// Allow CTRL-BRK during game init
//
// Revision 1.3  1998/02/03  01:32:58  stan
// Moved __djgpp_nearptr_enable() call from I_video.c to i_main.c
//
// Revision 1.2  1998/01/26  19:23:24  phares
// First rev with no ^Ms
//
// Revision 1.1.1.1  1998/01/19  14:02:57  rand
// Lee's Jan 19 sources
//
//----------------------------------------------------------------------------
