/*----------------------------------------------------------------------------------------------*/
/*! \file primo.h
//
// FILENAME: primo.h
//
// DESCRIPTION: Defines the 'nslPrimo' class.
//
// DEVELOPERS: Michele Bavaro (2010)
//
// LICENSE TERMS: Copyright (c) Michele Bavaro 2010
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
// Note:  Comments within this file follow a syntax that is compatible with
//        DOXYGEN and are utilized for automated document extraction
//
// Reference:
*/
/*----------------------------------------------------------------------------------------------*/


#ifndef PRIMO_H_
#define PRIMO_H_


/* Includes */
/*--------------------------------------------------------------*/
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
/*--------------------------------------------------------------*/

#include "nslprimo_api.h"

#define PRIMO_SAMP_MS (5456)


/*--------------------------------------------------------------*/
/*! \ingroup CLASSES
 *
 */
class nslPrimo
{

 private:

 public:

  nslPrimo();   //!< Constructor
  ~nslPrimo();    //!< Destructor

  void Start();   //!< Function to start
  int Read(uint8_t *b); //!< Function to read

  int ReadSize() { return NSLPRIMO_DATABUFF_SIZE; }

};
/*--------------------------------------------------------------*/


#endif /* PRIMO_H_ */
