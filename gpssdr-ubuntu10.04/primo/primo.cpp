/*----------------------------------------------------------------------------*/
/*! \file primo.cpp
//
// FILENAME: primo.cpp
//
// DESCRIPTION: Impelements the nslPrimo class.
//
// DEVELOPERS: Michele Bavaro (2003-2009)
//
// LICENSE TERMS: Copyright (c) Michele Bavaro 2009
//
// This file is part of the GPS Software Defined Radio (GPS-SDR)
//
// The GPS-SDR is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License,
// or (at your option) any later version.
// The GPS-SDR is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// Note:  Comments within this file follow a syntax that is compatible with
//        DOXYGEN and are utilized for automated document extraction
//
// Reference:
*/
/*----------------------------------------------------------------------------*/


/* Includes */
/*--------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include "nslprimo_api.h"
#include "primo.h"


static const char firmwareFileName[] = "./primo/fx2fw_gps-sdr.ihx";
static const char max2769FileName[]  = "./primo/max2769_gps-sdr.txt";


/*----------------------------------------------------------------------------*/
nslPrimo::nslPrimo()
{
  FILE *fwFileId, *max2769FileId;
  max2769Conf_t confResult;
  int ret;

  NSLPRIMO_InitLibrary();

  /*!
   * CHECK IF CONNECTED
   */
  if ( !NSLPRIMO_IsConnected() ) {
    fprintf(stdout, "Could not find a PRIMO device\n");
    exit(-1);
  }

  /*!
   * FIRMWARE UPLOAD SECTION
   */
  if ( !NSLPRIMO_IsConfigured() ) {
    fwFileId = fopen(firmwareFileName, "rt");
    if (NULL == fwFileId) {
      fprintf(stdout, "WARN: Firmware file not present in folder, exiting\n");
      exit(-1);
    }
    ret = NSLPRIMO_LoadFirmware(fwFileId);
    if (ret < 0) {
      fprintf(stdout, "%s\n", NSLPRIMO_Perror());
      exit(-1);
    }
    if(NULL!=fwFileId) {
      fclose(fwFileId);
    }
    sleep(1);
  }

  /*!
   * MAX2769 CONFIGURATION SECTION
   */
  max2769FileId = fopen(max2769FileName, "rt");
  if (NULL == max2769FileId) {
    fprintf(stdout, "Could not open the max2769 conf. file, exiting\n");
    exit(-1);
  }
  ret = NSLPRIMO_ConfigureMax2769(max2769FileId, &confResult);
  if (ret < 0) {
    fprintf(stdout, "%s\n", NSLPRIMO_Perror());
  }
  if(NULL != max2769FileId) {
    fclose(max2769FileId);
  }

  /*!
   * INIT SIGNAL GRABBER
   */
  ret = NSLPRIMO_GrabInit();
  if (ret < 0) {
    fprintf(stdout, "%s\n", NSLPRIMO_Perror());
  }

}
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
nslPrimo::~nslPrimo()
{
  int ret;
  ret = NSLPRIMO_GrabStop();
  if (ret < 0) {
    fprintf(stdout, "%s\n", NSLPRIMO_Perror());
  }
  NSLPRIMO_GrabClean();

  NSLPRIMO_QuitLibrary();
}
/*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
void nslPrimo::Start()
{
  int ret;

  ret = NSLPRIMO_GrabStart();
  if (ret < 0) {
    fprintf(stdout, "%s\n", NSLPRIMO_Perror());
  }


}
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
int nslPrimo::Read(uint8_t *buff)
{
  int ret;

  ret = NSLPRIMO_RefillDataBuffer();
  if (ret < 0) {
    fprintf(stdout, "\nBuffer overrun\n");
    fflush(stdout);
  }

  // Copy data in the buffer
  memcpy(buff, NSLPRIMO_dataBuffer,
   NSLPRIMO_DATABUFF_SIZE*sizeof(uint8_t));

  return NSLPRIMO_DATABUFF_SIZE;
}
/*----------------------------------------------------------------------------*/

