/******************************************************************************
  \file phySet.h

  \brief Sets PHY PIB parameter

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      11/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _PHYSET_H
#define _PHYSET_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <phyCommon.h>
#include <machwdSet.h>
#include <macphyPib.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/

/******************************************************************************
                        Types section.
******************************************************************************/
// PIB identifiers.
typedef enum
{
  // PHY id.
  PHY_SET_PIB_CURRENT_CHANNEL_ID     = PHY_PIB_CURRENT_CHANNEL_ID,
  PHY_SET_PIB_TRANSMIT_POWER_ID      = PHY_PIB_TRANSMIT_POWER_ID,
  PHY_SET_PIB_CCA_MODE_ID            = PHY_PIB_CCA_MODE_ID,
  PHY_SET_PIB_CURRENT_PAGE_ID        = PHY_PIB_CURRENT_PAGE_ID,
  // MAC id.
  PHY_SET_PIB_MAX_CSMA_BACKOFFS_ID   = MAC_PIB_MAX_CSMA_BACKOFFS_ID,
  PHY_SET_PIB_PANID_ID               = MAC_PIB_PANID_ID,
  PHY_SET_PIB_SHORT_ADDR_ID          = MAC_PIB_SHORT_ADDR_ID,
  PHY_SET_PIB_MIN_BE_ID              = MAC_PIB_MIN_BE_ID,
  PHY_SET_PIB_MAX_FRAME_RETRIES_ID   = MAC_PIB_MAX_FRAME_RETRIES_ID,
  PHY_SET_PIB_EXT_ADDR_ID            = MAC_PIB_EXT_ADDR_ID,
  // Additional id for software needs.
  PHY_SET_PIB_RF_IRQ_DISABLE_ID      = MACHWD_PIB_RF_IRQ_DISABLE_ID,
  PHY_SET_PIB_PAN_COORDINATOR_ID     = MACHWD_PIB_PAN_COORDINATOR_ID
} PHY_SetPibId_t;

typedef struct
{
  PHY_SetPibId_t    id;
  MACHWD_PibAttr_t  attr;
} PHY_SetReq_t;

/******************************************************************************
                        Prototypes section.
******************************************************************************/
/******************************************************************************
  Performs parameter's set operation to the RF chip.
  Parameters:
    setReq - phy ser request's parameters.
  Returns: none.
******************************************************************************/
PHY_ReqStatus_t PHY_SetReq(PHY_SetReq_t *setReq);

#endif /* _PHYSET_H */

// eof phySet.c
