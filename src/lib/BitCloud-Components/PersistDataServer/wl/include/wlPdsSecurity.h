/**************************************************************************//**
  \file wlPdsSecurity.h

  \brief PDS security interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    04.06.13 N. Fomin  - Created.
******************************************************************************/

#ifndef _WLPDSSECURITY_H_
#define _WLPDSSECURITY_H_
/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <wlPdsBindings.h>
#include <S_Nv.h>

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
\brief Stores item under security control

\param[in] id   - item id;
\param[in] size - item size;
\param[in] data - item data
******************************************************************************/
void pdsStoreSecuredItem(S_Nv_ItemId_t id, uint16_t size, void *data);

/******************************************************************************
\brief Restores item under security control

\param[in] id   - item id;
\param[in] size - item size;
\param[in] data - item data

\returns true if item is restored successfully, false - otherwise
******************************************************************************/
bool pdsRestoreSecuredItem(S_Nv_ItemId_t id, uint16_t size, void *data);

#endif /* _WLPDSSECURITY_H_ */
/* eof wlPdsTaskManager.h */

