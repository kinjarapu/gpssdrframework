/*----------------------------------------------------------------------------------------------*/
/*! \file keyboard.cpp
//
// FILENAME: keyboard.cpp
//
// DESCRIPTION: Defines the keyboard class.
//
// DEVELOPERS: Gregory W. Heckler (2003-2009)
//
// LICENSE TERMS: Copyright (c) Gregory W. Heckler 2009
//
// This file is part of the GPS Software Defined Radio (GPS-SDR)
//
// The GPS-SDR is free software; you can redistribute it and/or modify it under the terms of the
// GNU General Public License as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version. The GPS-SDR is distributed in the hope that
// it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// Note:	Comments within this file follow a syntax that is compatible with
//			DOXYGEN and are utilized for automated document extraction
//
// Reference:
*/
/*----------------------------------------------------------------------------------------------*/

#include "patience.h"

/*----------------------------------------------------------------------------------------------*/
void *Patience_Thread(void *_arg)
{
	Patience *aPatience = pPatience;

	while(grun)
		aPatience->PetMe();

	pthread_exit(0);
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void Patience::Start()
{
	Start_Thread(Patience_Thread, NULL);

	if(gopt.verbose)
		fprintf(stdout,"Patience thread started\n");
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
Patience::Patience():Threaded_Object("KEYTASK")
{
	last_tic = 0;

	if(gopt.verbose)
		fprintf(stdout,"Creating Patience\n");
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
Patience::~Patience()
{
	if(gopt.verbose)
		fprintf(stdout,"Destructing Patience\n");
}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
void Patience::PetMe()
{
	uint32_t new_tic;
	//following two lines added by azimout
	time_t rawtime;
	struct tm * timeinfo;

	if(pFIFO != NULL)
	{
		new_tic = pFIFO->getExecTic();
		if(new_tic == last_tic)
		{
			// next three lines added by azimout to print watchdog trigger time
			time(&rawtime);
			timeinfo = localtime (&rawtime);
			fprintf(stdout, "Watchdog tripped at time %s\n", asctime(timeinfo));
			pFIFO->Stop();
			pFIFO->ResetSource();
			pFIFO->Start();
			last_tic = 0;
		}
		else
		{
			last_tic = new_tic;
		}
	}

	sleep(1);
}
/*----------------------------------------------------------------------------------------------*/
