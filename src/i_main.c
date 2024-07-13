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

#include "m_fixed.h"

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

fixed_t rand_fixed()
{
    fixed_t a = rand();
    a <<= 15;
    a |= rand();
    a <<= 2;
    a |= rand() % 4;
    return a;
}

inline boolean mismatch(fixed_t a, fixed_t b)
{
    return FixedDiv(a,b) != FixedDiv_old(a,b) || FixedDiv(b,a) != FixedDiv_old(b,a);
}

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

   if (mismatch(INT_MIN, INT_MIN) ||
       mismatch(INT_MAX, INT_MAX) ||
       mismatch(INT_MIN, INT_MAX) ||
       mismatch(INT_MIN, -1) ||
       mismatch(INT_MAX, -1))
   {
       printf("Extremal values test failed\n");
       exit(1);
   }

   unsigned seed = 1337;
   srand(seed);

   printf("Testing FixedDiv with seed=%u\n", seed);

   Uint64 start = SDL_GetPerformanceCounter();

   fixed_t a_first, b_first;
   for (unsigned i = 0; i < UINT_MAX; ++i)
   {
       fixed_t a = rand_fixed();
       fixed_t b = rand_fixed();

       if (i == 0)
       {
	   a_first = a;
	   b_first = b;
       }
       else if (a == a_first && b == b_first)
       {
	   break;
       }

       fixed_t fd = FixedDiv(a,b);
       fixed_t fd_old = FixedDiv_old(a,b);

       if (fd != fd_old)
       {
	   printf("Mismatch at iteration %u: a = %d, b = %d\n", i, a, b);
	   printf("FixedDiv result = %d\n", fd_old);
	   printf("FixedDiv (div64) result = %d\n", fd);
	   exit(1);
       }

       if (i && (i % (1024 * 1024 * 8) == 0))
	   printf("Completed %u iterations\n", i);
   }

   Uint64 end = SDL_GetPerformanceCounter();
   unsigned et = (end - start) / SDL_GetPerformanceFrequency();

   printf("Testing completed after %u seconds\n", et);

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
