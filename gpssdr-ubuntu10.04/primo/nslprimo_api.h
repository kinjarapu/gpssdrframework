/*! \file NSLPRIMO_api.h
 *  \brief Libraries entry points
 *
 * Project: NSLPRIMO
 * Autor: M.Bavaro
 * Revision:  1.0
 * Rev. Date: 17.12.08
 * Dev. Tool:   GNU C
 * Target CPU:  Intel Pentium 4
 * Description:
 *-------------------------------------------------------------------------
 * Modification log:
 * Rev.    Modif.      Date        Name
 * 1.0     None        17.12.08    First revision
 *-------------------------------------------------------------------------
 */
#ifndef _NSLPRIMO_API_INCLUDED_
#define _NSLPRIMO_API_INCLUDED_

#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif


//typedef unsigned char uint8;

/*! \struct
 *  \brief
 */
typedef enum _bool_e {F=0, T} bool_t;


/*! \struct
 *  \brief
 */
typedef struct _max2769Conf_t {
  unsigned confOne;
  unsigned confTwo;
  unsigned confThree;
  unsigned pllConf;
  unsigned nrDiv;
  unsigned fDiv;
  unsigned strm;
  unsigned clk;
  unsigned testOne;
  unsigned testTwo;
} max2769Conf_t;

/*! \var unsigned NSLPRIMO_DATABUFF_SIZE
 *  \
 */
extern const unsigned NSLPRIMO_DATABUFF_SIZE;

/*! \var unsigned char NSLPRIMO_dataBuffer[]
 *  \
 */
extern unsigned char NSLPRIMO_dataBuffer[];


/*! \fn int NSLBUG_InitLibrary
 *  \brief
 */
int NSLPRIMO_InitLibrary( void );

/*! \fn int NSLBUG_QuitLibrary
 *  \brief
 */
void NSLPRIMO_QuitLibrary( void );


/*! \fn char* NSLPRIMO_Perror
 *  \brief
 */
char* NSLPRIMO_Perror( void );

/*! \fn int NSLPRIMO_LoadFirmware
 *  \brief
 */
int NSLPRIMO_LoadFirmware( FILE *fid );

/*! \fn bool_t NSLPRIMO_IsConfigured
 *  \brief
 */
bool_t NSLPRIMO_IsConfigured( void );

/*! \fn bool_t NSLPRIMO_IsConnected
 *  \brief
 */
bool_t NSLPRIMO_IsConnected( void );


/*! \fn int NSLPRIMO_ConfigureMax2769
 *  \brief
 */
int NSLPRIMO_ConfigureMax2769( FILE *fid, max2769Conf_t *pResult);


/*! \fn int NSLPRIMO_GrabInit
 *  \brief
 */
int NSLPRIMO_GrabInit( void );


/*! \fn int NSLPRIMO_GrabStart
 *  \brief
 */
int NSLPRIMO_GrabStart( void );


/*! \fn int NSLPRIMO_RefillDataBuffer
 *  \brief
 */
int NSLPRIMO_RefillDataBuffer( void );


/*! \fn int NSLPRIMO_GrabStop
 *  \brief
 */
int NSLPRIMO_GrabStop( void );


/*! \fn int NSLPRIMO_GrabClean
 *  \brief
 */
void NSLPRIMO_GrabClean( void );


#ifdef __cplusplus
}
#endif


#endif /* _NSLPRIMO_API_INCLUDED_ */
