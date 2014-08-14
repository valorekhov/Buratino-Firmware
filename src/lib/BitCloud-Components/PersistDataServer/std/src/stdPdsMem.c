/**************************************************************************//**
  \file pdsMem.c

  \brief Memory of Persistent Data Server

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-01-16 A. Razinkov  - Created.
   Last change:
    $Id: stdPdsMem.c 25824 2013-12-10 08:44:12Z unithra.c $
 ******************************************************************************/

#if PDS_ENABLE_WEAR_LEVELING != 1
#ifdef _ENABLE_PERSISTENT_SERVER_
/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <stdPdsMem.h>

/******************************************************************************
                           Global variables section
 ******************************************************************************/
PdsMem_t pdsMem;
#endif /* _ENABLE_PERSISTENT_SERVER_ */
#endif /* PDS_ENABLE_WEAR_LEVELING != 1 */
/* eof pdsMem.c */

