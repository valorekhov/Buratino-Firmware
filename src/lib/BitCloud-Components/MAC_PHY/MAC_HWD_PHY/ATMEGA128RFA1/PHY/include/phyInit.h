/******************************************************************************
  \file phyInit.h

  \brief Radio Transciever Initialization.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      07/04/08 A. Mandychev - Created.
******************************************************************************/

#ifndef _PHYINIT_H
#define _PHYINIT_H

/******************************************************************************
                    Includes section
******************************************************************************/

/******************************************************************************
                    Define(s) section
******************************************************************************/
#define PHY_Init RF_Init

/******************************************************************************
                    Types section
******************************************************************************/

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  Initializes radio transciever.
  Parameters: none.
  Returns: status operation.
******************************************************************************/
bool PHY_Init(void);

/******************************************************************************
  Set external frequency generated by RF chip.
  Parameters: none.
  Returns: none.
******************************************************************************/
void phySetFreq(void);

#endif /*_PHYINIT_H*/

// eof phyInit.h
