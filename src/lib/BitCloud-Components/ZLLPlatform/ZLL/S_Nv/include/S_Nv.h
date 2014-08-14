/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: S_Nv.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef S_NV_H
#define S_NV_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Types.h"


/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/

/** The ranges of id's that can be used by
    the application for storing flash data
*/
#define S_NV_APPLICATION_RANGE_MIN 0x0900u
#define S_NV_APPLICATION_RANGE_MAX 0x3FFFu

/** The ranges of id's that can be used by
    the platform for storing flash data
*/
#define S_NV_PLATFORM_RANGE_MIN 0x0800u
#define S_NV_PLATFORM_RANGE_MAX 0x08FFu

/** Identifier type to an item in Flash. Valid range is 1..32767. */
typedef uint16_t S_Nv_ItemId_t;

/** Return value type of most S_Nv functions. */
typedef enum S_Nv_ReturnValue_t
{
    /** The operation succeeded. */
    S_Nv_ReturnValue_Ok,

    /** Unable to write to flash. */
    S_Nv_ReturnValue_Failure,

    /** The item didn't exist but was successfully created and initialised. No error. */
    S_Nv_ReturnValue_DidNotExist,

    /** The item that was attempted to access doesn't exist. */
    S_Nv_ReturnValue_DoesNotExist,

    /** An attempt was made to read or write beyond the length of the item. */
    S_Nv_ReturnValue_BeyondEnd,

    /** The power supply voltage was too low to reliably perform the requested operation. The operation has not been executed. */
    S_Nv_ReturnValue_PowerSupplyTooLow,

} S_Nv_ReturnValue_t;

/** Function type used to check the external NV's power supply.
    Should only return TRUE if the power supply is high enough to reliably perform a write or erase operation.
*/
typedef bool (*S_Nv_PowerSupplyCheckingFunction_t)(void);

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/

/** The ranges of id's that can be used by
    the application for storing flash data
*/
#define S_NV_APPLICATION_RANGE_MIN 0x0900u
#define S_NV_APPLICATION_RANGE_MAX 0x3FFFu

/** The ranges of id's that can be used by
    the platform for storing flash data
*/
#define S_NV_PLATFORM_RANGE_MIN 0x0800u
#define S_NV_PLATFORM_RANGE_MAX 0x08FFu

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Initializes an item with default data if it doesn't exist yet, or reads it when it does exist.

    \param id The item to initialize.
    \param itemLength The length of the item.
    \param pDefaultData Buffer to 'itemLength' bytes, or NULL.
    \returns S_Nv_ReturnValue_Ok, S_Nv_ReturnValue_DidNotExist or S_Nv_ReturnValue_PowerSupplyTooLow.

    If the item doesn't exist, it is written with the given default data and the status code
    S_Nv_ReturnValue_DidNotExist is returned. Otherwise, the default data is not used, the item
    is kept as it was but read into pDefaultData, and S_Nv_ReturnValue_Ok is returned.
    If NULL is passed for pDefaultData, and the item doesn't exist all data is initialized to 0xFF.

    \note pDefaultData must not point to constant data.
*/
S_Nv_ReturnValue_t S_Nv_ItemInit(S_Nv_ItemId_t id, uint16_t itemLength, void* pDefaultData);

/** Writes an item.

    \param id The item to write.
    \param offset The offset in the item to start writing to.
    \param dataLength The length of the data to write.
    \param pData Pointer to 'dataLength' bytes to write.
    \returns S_Nv_ReturnValue_Ok, S_Nv_ReturnValue_DoesNotExist, S_Nv_ReturnValue_BeyondEnd or S_Nv_ReturnValue_PowerSupplyTooLow.

    If any of the bytes fall outside the item lenght as passed during S_Nv_ItemInit(), the whole
    write operation does not occur and S_Nv_ReturnValue_BeyondEnd is returned.
*/
S_Nv_ReturnValue_t S_Nv_Write(S_Nv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData);

/** Reads an item.

    \param id The item to read.
    \param offset The offset in the item to start reading from.
    \param dataLength The length of the data to read.
    \param  pData Pointer to a buffer where the read bytes are to be written to.
    \returns S_Nv_ReturnValue_Ok, S_Nv_ReturnValue_DoesntExist or S_Nv_ReturnValue_BeyondEnd.

    If any of the bytes fall outside the item lenght as passed during S_Nv_ItemInit(), the whole
    read operation does not occur and S_Nv_ReturnValue_BeyondEnd is returned.
*/

S_Nv_ReturnValue_t S_Nv_Read(S_Nv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData);

/** Returns the item length of an item.

    \param id The item to get the length from.
    \returns The length of the item, in bytes, as passed during S_Nv_ItemInit(), or 0 if the item doesn't exist.
*/
uint16_t S_Nv_ItemLength(S_Nv_ItemId_t id);

/** Deletes an item from non-volatile storage.

    \param id The item to delete.
    \returns S_Nv_ReturnValue_Ok, S_Nv_ReturnValue_DoesntExist or S_Nv_ReturnValue_PowerSupplyTooLow.
*/
S_Nv_ReturnValue_t S_Nv_Delete(S_Nv_ItemId_t id);

/** Erases all items (optionally except the persistent ones) from non-volatile storage.

    \param includingPersistentItems Whether to also erase the peristent items.
    \returns S_Nv_ReturnValue_Ok or S_Nv_ReturnValue_PowerSupplyTooLow.

    The persistent items should typically never be erased in an application!
*/
S_Nv_ReturnValue_t S_Nv_EraseAll(bool includingPersistentItems);

/** This will check whether the item is found in the storage area.

    \param id - Id of the item to be checked.
    \returns true or false
*/
bool S_Nv_IsItemAvailable(S_Nv_ItemId_t id);

/** Sets a function to check the power supply.

    \param pf A pointer to a function that S_Nv uses to check the power supply.

    If the power supply checking function is not set, the default is to assume that the power
    supply is always high enough. This is not suitable for typical applications.
    When this function is called again,
*/
void S_Nv_SetPowerSupplyCheckingFunction(S_Nv_PowerSupplyCheckingFunction_t pf);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // S_NV_H

