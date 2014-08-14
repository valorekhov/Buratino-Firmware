/**************************************************************************//**
\file  ofdCommand.c

\brief Implementation of OTAU flash driver interface.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    7/08/09 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#if APP_USE_OTAU == 1
#if (APP_USE_OTAU == 1)
#if APP_USE_FAKE_OFD_DRIVER == 0

/******************************************************************************
                   Includes section
******************************************************************************/
#include <ofdExtMemory.h>
#include <ofdMemoryDriver.h>
#include <driversDbg.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define CRC_INITIALIZATION_VALUE     0xFF

/******************************************************************************
                   Global variables section
******************************************************************************/
OFD_Position_t ofdPosition = 0;
OFD_Position_t actionSector = 0;
OFD_Callback_t ofdCallback = NULL;
OFD_Callback_t ofdAuxCallback = NULL;
OFD_MemoryAccessParam_t localAccessStructure;
OfdInternalMemoryAccessParam_t internalAccessParam;
OFD_ImageInfo_t imageInfo;
OfdImageTable_t imageTable;

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Erases image in the external memory.

\param[in]
  pos - image position in the external memory
\param[in]
  cb - pointer to callback
******************************************************************************/
void OFD_EraseImage(OFD_Position_t pos, OFD_Callback_t cb)
{
  if (pos >= OFD_POSITION_MAX)
  {
    sysAssert(cb, OFD_NULLCALLBACK2);
    cb(OFD_STATUS_INCORRECT_API_PARAMETER);
    return;
  }

  ofdPosition = pos;
  ofdCallback = cb;
  ofdEraseImage();
}

/**************************************************************************//**
\brief Writes data to the external memory.

\param[in]
  pos - image position for new data
\param[in]
  accessParam - pointer to the access structure
\param[in]
  cb - pointer to callback
******************************************************************************/
void OFD_Write(OFD_Position_t pos, OFD_MemoryAccessParam_t *accessParam, OFD_Callback_t cb)
{
  if ((pos >= OFD_POSITION_MAX) || (NULL == accessParam))
  {
    sysAssert(cb, OFD_NULLCALLBACK17);
    cb(OFD_STATUS_INCORRECT_API_PARAMETER);
    return;
  }

  ofdPosition = pos;
  ofdCallback = cb;
  localAccessStructure = *accessParam;

  // set table info like "image was written through api"
  imageTable |= (OfdImageTable_t)(1 << pos);

  if (localAccessStructure.offset >= OFD_START_EEPROM_SREC_ADDRESS)
    localAccessStructure.offset -= (OFD_START_EEPROM_SREC_ADDRESS - OFD_EEPROM_OFFSET_WITHIN_IMAGE);

  if (OFD_POSITION_1 == ofdPosition)
    localAccessStructure.offset += OFD_IMAGE1_START_ADDRESS;
  else
    localAccessStructure.offset += OFD_IMAGE2_START_ADDRESS;

  ofdWriteData();
}

/**************************************************************************//**
\brief Flushes data from internal buffer, checks image crc and saves it to
the external memory.

\param[in]
  pos - image position for new data
\param[in]
  countBuff - pointer to the memory for internal data (memory size must be OFD_BLOCK_FOR_CHECK_CRC)
\param[in]
  cb - pointer to callback
******************************************************************************/
void OFD_FlushAndCheckCrc(OFD_Position_t pos, uint8_t *countBuff, OFD_InfoCallback_t cb)
{
  if ((pos >= OFD_POSITION_MAX) || (NULL == countBuff))
  {
    sysAssert(cb, OFD_NULLCALLBACK18);
    cb(OFD_STATUS_INCORRECT_API_PARAMETER, &imageInfo);
    return;
  }

  ofdFlushData();

  ofdPosition = pos;
  ofdCallback = (OFD_Callback_t)cb;
  imageInfo.crc = CRC_INITIALIZATION_VALUE;

  localAccessStructure.data = countBuff;
  localAccessStructure.length = OFD_IMAGE_SIZE;
  if (OFD_POSITION_1 == ofdPosition)
    localAccessStructure.offset = OFD_IMAGE1_START_ADDRESS;
  else
    localAccessStructure.offset = OFD_IMAGE2_START_ADDRESS;

  ofdCheckCrc();
}

/**************************************************************************//**
\brief Saves current mcu flash and eeprom to the external memory, checks crc for its
and set command for bootloader.

\param[in]
  whereToSave - image position for current mcu flash and eeprom
\param[in]
  from        - new image position
\param[in]
  copyBuff - pointer to the memory for internal data (memory size must be OFD_BLOCK_FOR_CHECK_CRC)
\param[in]
  cb - pointer to callback
******************************************************************************/
void OFD_SwitchToNewImage(OFD_Position_t whereToSave, OFD_Position_t from, uint8_t *copyBuff, OFD_Callback_t cb)
{
  if ((whereToSave >= OFD_POSITION_MAX) || (from >= OFD_POSITION_MAX) || (whereToSave == from) || (NULL == copyBuff))
  {
    sysAssert(cb, OFD_NULLCALLBACK19);
    cb(OFD_STATUS_INCORRECT_API_PARAMETER);
    return;
  }

  ofdPosition = whereToSave;
  actionSector = from;
  ofdAuxCallback = cb;

  // set table info like "image was saved from mcu"
  imageTable &= (OfdImageTable_t)~(1 << ofdPosition);

  internalAccessParam.flashOffset = 0ul;
  internalAccessParam.data = copyBuff;
  internalAccessParam.length = OFD_IMAGE_SIZE;

  if (OFD_POSITION_1 == ofdPosition)
    localAccessStructure.offset = OFD_IMAGE1_START_ADDRESS;
  else
    localAccessStructure.offset = OFD_IMAGE2_START_ADDRESS;

  ofdSaveCurrentFlashImage();
}

/**************************************************************************//**
\brief Reads image informations.

\param[in]
  pos - image position
\param[in]
  cb - pointer to callback
******************************************************************************/
void OFD_ReadImageInfo(OFD_Position_t pos, OFD_InfoCallback_t cb)
{
  if (pos >= OFD_POSITION_MAX)
  {
    sysAssert(cb, OFD_NULLCALLBACK21);
    cb(OFD_STATUS_INCORRECT_API_PARAMETER, &imageInfo);
    return;
  }

  ofdPosition = pos;

  if (imageTable & (1 << pos))
    imageInfo.type = OFD_IMAGE_WAS_WRITTEN_THROUGH_API;
  else
    imageInfo.type = OFD_IMAGE_WAS_SAVED_FROM_MCU;

  ofdCallback = (OFD_Callback_t)cb;

  ofdReadCrc();
}

/**************************************************************************//**
\brief Sets command for bootloader.

\param[in]
  from        - image position
\param[in]
  cb - pointer to callback
******************************************************************************/
void OFD_ChangeImage(OFD_Position_t from, OFD_Callback_t cb)
{
  if (from >= OFD_POSITION_MAX)
  {
    sysAssert(cb, OFD_NULLCALLBACK20);
    cb(OFD_STATUS_INCORRECT_API_PARAMETER);
    return;
  }

  actionSector = from;
  ofdAuxCallback = cb;
  ofdSetActionForBootloader();
}

#endif // APP_USE_FAKE_OFD_DRIVER == 0
#endif // (APP_USE_OTAU == 1)
#endif // APP_USE_OTAU == 1

// eof ofdCommand.c
